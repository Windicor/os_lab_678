#include "server.h"

#include <iostream>

#include "m_zmq.h"

using namespace std;

Server::Server() {
  pid_ = getpid();
  cerr << pid_ << " Starting server..." << endl;
  context_ = create_zmq_context();

  cerr << "Server is started correctly" << endl;
}

Server::~Server() {
  cerr << pid_ << " Destroying server..." << endl;

  destroy_zmq_context(context_);

  cerr << "Server is destroyed correctly" << endl;
}

pid_t Server::pid() const {
  return pid_;
}