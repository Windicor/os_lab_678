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
    pid_t pid = fork();
    if (pid == -1) {
      throw runtime_error("Can't fork");
    }
    if (pid == 0) {
      execl(CLIENT_EXE.data(), CLIENT_EXE.data(), NULL);
      kill(server_pid, SIGABRT);
      exit(ERR_EXEC);
    }
    //cerr << "My id is " << server_ptr->pid() << " child id is " << pid << endl;

  } catch (exception& ex) {
    cerr << "Exctption: " << ex.what() << "\nTerminated by exception on server receive loop" << endl;
    exit(ERR_LOOP);
  }
  return NULL;
}

Server::Server() {
  pid_ = getpid();
  cerr << to_string(pid_) + " Starting server..."s << endl;
  context_ = create_zmq_context();

  string endpoint_ = create_endpoint(EndpointType::CHILD_PUB, getpid());
  publiser_ = make_unique<Socket>(context_, SocketType::PUBLISHER, ConnectionType::BIND, endpoint_);

  if (pthread_create(&receive_msg_loop_id, 0, receive_msg_loop, this) != 0) {
    throw runtime_error("Can't run second thread");
  }
}

Server::~Server() {
  cerr << to_string(pid_) + " Destroying server..."s << endl;

  publiser_ = nullptr;
  subscriber_ = nullptr;
  destroy_zmq_context(context_);
}

pid_t Server::pid() const {
  return pid_;
}