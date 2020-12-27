#include "server.h"

#include <iostream>

#include "m_zmq.h"

using namespace std;

Server::Server() {
  pid_ = getpid();
  cerr << pid_ << " Starting server..." << endl;
  context_ = create_zmq_context();
  publiser_ = make_unique<Socket>(context_, SocketType::PUBLISHER, EndpointType::CHILD_PUB, ConnectionType::BIND);

  //sub_socket_ = create_zmq_socket(context_, SocketType::SUBSCRIBER);
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