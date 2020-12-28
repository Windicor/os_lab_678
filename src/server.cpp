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

void* receive_msg_loop(void* serv_arg) {
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
      cout << "Message on server: " << static_cast<int>(msg.command) << " " << msg.to_id << " " << msg.value << endl;
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

  if (pthread_create(&receive_msg_loop_id, 0, receive_msg_loop, this) != 0) {
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

  try {
    publiser_ = nullptr;
    subscriber_ = nullptr;
    destroy_zmq_context(context_);
  } catch (exception& ex) {
    cerr << "Server wasn't destroyed: " << ex.what() << endl;
  }
}

void Server::send(Message message) {
  publiser_->send(message);
}

Message Server::receive() {
  return subscriber_->receive();
}

pid_t Server::pid() const {
  return pid_;
}