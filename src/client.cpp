#include "client.h"

#include <unistd.h>

#include <iostream>

#include "m_zmq.h"

using namespace std;

const string CLIENT_EXE = "./client";
const double MESSAGE_WAITING_TIME = 1;

Client::Client(int id, std::string parrent_endpoint) {
  id_ = id;
  pid_ = getpid();
  cerr << to_string(pid_) + " Starting client..."s << endl;
  context_ = create_zmq_context();

  string endpoint = create_endpoint(EndpointType::CHILD_PUB, getpid());
  child_publiser_ = make_unique<Socket>(context_, SocketType::PUBLISHER, endpoint);
  endpoint = create_endpoint(EndpointType::PARRENT_PUB, getpid());
  parrent_publiser_ = make_unique<Socket>(context_, SocketType::PUBLISHER, endpoint);

  subscriber_ = make_unique<Socket>(context_, SocketType::SUBSCRIBER, parrent_endpoint);

  sleep(MESSAGE_WAITING_TIME);
  send_up(Message(CommandType::CREATE_CHILD, id, getpid()));
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
    cerr << to_string(pid_) + " " + " Client wasn't destroyed: "s << ex.what() << endl;
  }
}

void Client::send_up(Message message) {
  message.go_up = true;
  parrent_publiser_->send(message);
}

void Client::send_down(Message message) {
  message.go_up = false;
  child_publiser_->send(message);
}

Message Client::receive() {
  Message msg = subscriber_->receive();
  return msg;
}

void Client::add_child(int id) {
  pid_t child_pid = fork();
  if (child_pid == -1) {
    throw runtime_error("Can't fork");
  }
  if (child_pid == 0) {
    execl(CLIENT_EXE.data(), CLIENT_EXE.data(), to_string(id).data(), child_publiser_->endpoint().data(), NULL);
    throw runtime_error("Can't execl ");
  }

  string endpoint = create_endpoint(EndpointType::PARRENT_PUB, child_pid);
  subscriber_->subscribe(endpoint);
}

int Client::id() const {
  return id_;
}

pid_t Client::pid() const {
  return pid_;
}