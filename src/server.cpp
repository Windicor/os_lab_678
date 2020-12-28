#include "server.h"

#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>

#include "m_zmq.h"

using namespace std;

const int ERR_LOOP = 2;
const int ERR_EXEC = 3;
const string CLIENT_EXE = "./client";
const double MESSAGE_WAITING_TIME = 1;
const int UNIVERSAL_MESSAGE_ID = -256;

void* second_thread(void* serv_arg) {
  Server* server_ptr = (Server*)serv_arg;
  pid_t server_pid = server_ptr->pid();
  try {
    pid_t child_pid = fork();
    if (child_pid == -1) {
      throw runtime_error("Can't fork");
    }
    if (child_pid == 0) {
      execl(CLIENT_EXE.data(), CLIENT_EXE.data(), "0", server_ptr->publiser_->endpoint().data(), NULL);
      cerr << "Can't execl "s + CLIENT_EXE << endl;
      kill(server_pid, SIGINT);
      exit(ERR_EXEC);
    }

    string endpoint = create_endpoint(EndpointType::PARRENT_PUB, child_pid);
    server_ptr->subscriber_ = make_unique<Socket>(server_ptr->context_, SocketType::SUBSCRIBER, endpoint);
    server_ptr->tree_.add_to(0, {0, child_pid});

    while (true) {
      Message msg = server_ptr->subscriber_->receive();
      if (msg.command == CommandType::ERROR) {
        if (server_ptr->terminated_) {
          return NULL;
        } else {
          cerr << "This bom" << endl;
          throw runtime_error("Can't receive message");
        }
      }
      server_ptr->last_message_ = msg;
      cerr << "Message on server: " << static_cast<int>(msg.command) << " " << msg.to_id << " " << msg.value << endl;

      switch (msg.command) {
        case CommandType::CREATE_CHILD: {
          auto& pa = server_ptr->tree_.get(msg.to_id);
          pa.second = msg.value;
          cout << "OK: " << server_ptr->last_message_.value << endl;
          break;
        }
        case CommandType::REMOVE_CHILD:
          server_ptr->tree_.remove(msg.to_id);
          cout << "OK" << endl;
          break;
        case CommandType::TIMER_START:
          cout << "OK:" << msg.to_id << endl;
          break;
        case CommandType::TIMER_STOP:
          cout << "OK:" << msg.to_id << endl;
          break;
        case CommandType::TIMER_TIME: {
          cout << "OK:" << msg.to_id << ": " << msg.value << endl;
          break;
        }
        case CommandType::HEARTBIT:
          server_ptr->map_for_check_[msg.to_id] = true;
          break;
        default:
          break;
      }
    }

  } catch (exception& ex) {
    cerr << "Server exctption: " << ex.what() << "\nTerminated by exception on server receive loop" << endl;
    exit(ERR_LOOP);
  }
  return NULL;
}

Server::Server() {
  pid_ = getpid();
  cerr << to_string(pid_) + " Starting server..."s << endl;
  context_ = create_zmq_context();

  string endpoint = create_endpoint(EndpointType::CHILD_PUB, getpid());
  publiser_ = make_unique<Socket>(context_, SocketType::PUBLISHER, endpoint);

  if (pthread_create(&receive_msg_loop_id, 0, second_thread, this) != 0) {
    throw runtime_error("Can't run second thread");
  }
}

Server::~Server() {
  if (terminated_) {
    cerr << to_string(pid_) + " Server double termination" << endl;
    return;
  }

  cerr << to_string(pid_) + " Destroying server..."s << endl;
  terminated_ = true;

  for (pid_t pid : tree_.get_all_second()) {
    kill(pid, SIGINT);
  }

  try {
    publiser_ = nullptr;
    subscriber_ = nullptr;
    destroy_zmq_context(context_);
  } catch (exception& ex) {
    cerr << "Server wasn't destroyed: " << ex.what() << endl;
  }
}

void Server::send(Message message) {
  message.go_up = false;
  publiser_->send(message);
}

Message Server::receive() {
  return subscriber_->receive();
}

pid_t Server::pid() const {
  return pid_;
}

Message Server::last_message() const {
  return last_message_;
}

bool Server::check(int id) {
  Message msg(CommandType::RETURN, id, 0);
  send(msg);
  sleep(MESSAGE_WAITING_TIME);
  if (last_message_ == msg) {
    return true;
  } else {
    return false;
  }
}

void Server::create_child_cmd(int id, int parrent_id) {
  if (tree_.find(id)) {
    cout << "Error: Already exists" << endl;
    return;
  }
  if (!tree_.find(parrent_id)) {
    cout << "Error: Parent not found" << endl;
    return;
  }
  if (!check(parrent_id)) {
    cout << "Error: Parent is unavailable" << endl;
    return;
  }
  send(Message(CommandType::CREATE_CHILD, parrent_id, id));
  tree_.add_to(parrent_id, {id, 0});
}

void Server::remove_child_cmd(int id) {
  if (id == 0) {
    cout << "Can't remove zero child" << endl;
    return;
  }
  if (!tree_.find(id)) {
    cout << "Error: Not found" << endl;
    return;
  }
  if (!check(id)) {
    cout << "Error: Node is unavailable" << endl;
    return;
  }
  send(Message(CommandType::REMOVE_CHILD, id, 0));
}

void Server::exec_cmd(int id, CommandType type) {
  if (!tree_.find(id)) {
    cout << "Error: Not found" << endl;
    return;
  }
  if (!check(id)) {
    cout << "Error: Node is unavailable" << endl;
    return;
  }
  send(Message(type, id, 0));
}

void Server::heartbit_cmd(int time) {
  if (time < 1000) {
    cout << "Too low time for heartbit" << endl;
  }
  send(Message(CommandType::HEARTBIT, UNIVERSAL_MESSAGE_ID, time));
  auto uset = tree_.get_all_first();
  for (int id : uset) {
    map_for_check_[id] = false;
  }
  sleep(4 * (double)time / 1000);
  for (auto& [id, bit] : map_for_check_) {
    if (!bit) {
      cout << "Heartbit: node " << id << " is unavailable now" << endl;
    }
    bit = false;
  }
  cout << "OK" << endl;
}

void Server::print_tree() {
  tree_.print();
}