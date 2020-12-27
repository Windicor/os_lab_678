#include "server.h"

#include <pthread.h>

#include <iostream>

#include "m_zmq.h"

using namespace std;

void* receive_msg_loop(void* serv_arg) {
  Server* server_ptr = (Server*)serv_arg;
  for (int i = 0; i < 5; ++i) {
    cout << server_ptr->pid() << endl;
  }
  return NULL;
}

Server::Server() {
  pid_ = getpid();
  cerr << pid_ << " Starting server..." << endl;
  context_ = create_zmq_context();

  string endpoint_ = create_endpoint(EndpointType::CHILD_PUB, getpid());
  publiser_ = make_unique<Socket>(context_, SocketType::PUBLISHER, ConnectionType::BIND, endpoint_);

  if (pthread_create(&receive_msg_loop_id, 0, receive_msg_loop, this) != 0) {
    throw runtime_error("Can't run second thread");
  }
}

Server::~Server() {
  cerr << pid_ << " Destroying server..." << endl;

  publiser_ = nullptr;
  subscriber_ = nullptr;
  destroy_zmq_context(context_);
}

pid_t Server::pid() const {
  return pid_;
}