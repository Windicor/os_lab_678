#include "server.h"

#include <iostream>

#include "m_zmq.h"

using namespace std;

Server::Server() {
  pid_ = getpid();
  cerr << pid_ << " Starting server..." << endl;
  context_ = create_zmq_context();

  string endpoint_ = create_endpoint(EndpointType::CHILD_PUB, getpid());
  publiser_ = make_unique<Socket>(context_, SocketType::PUBLISHER, ConnectionType::BIND, endpoint_);

  //...
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