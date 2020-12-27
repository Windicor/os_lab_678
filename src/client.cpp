#include "client.h"

#include <unistd.h>

#include <iostream>

#include "m_zmq.h"

using namespace std;

Client::Client(int id, std::string parrent_endpoint) {
  id_ = id;
  pid_ = getpid();
  cerr << to_string(pid_) + " Starting client..."s << endl;
  context_ = create_zmq_context();

  string endpoint = create_endpoint(EndpointType::CHILD_PUB, getpid());
  child_publiser_ = make_unique<Socket>(context_, SocketType::PUBLISHER, ConnectionType::BIND, endpoint);
  endpoint = create_endpoint(EndpointType::PARRENT_PUB, getpid());
  parrent_publiser_ = make_unique<Socket>(context_, SocketType::PUBLISHER, ConnectionType::BIND, endpoint);

  subscriber_ = make_unique<Socket>(context_, SocketType::SUBSCRIBER, ConnectionType::CONNECT, parrent_endpoint);
}

Client::~Client() {
  if (terminated_) {
    cerr << to_string(pid_) + " Client double termination"s << endl;
    return;
  }

  cerr << to_string(pid_) + " Destroying client..."s << endl;
  terminated_ = true;

  try {
    child_publiser_ = nullptr;
    parrent_publiser_ = nullptr;
    subscriber_ = nullptr;
    destroy_zmq_context(context_);
  } catch (exception& ex) {
    cerr << to_string(pid_) + " Client wasn't destroyed: "s << ex.what() << endl;
  }
}

void Client::send_up(Message message) {
  parrent_publiser_->send(message);
}

void Client::send_down(Message message) {
  child_publiser_->send(message);
}

Message Client::receive() {
  return subscriber_->receive();
}

pid_t Client::pid() const {
  return pid_;
}