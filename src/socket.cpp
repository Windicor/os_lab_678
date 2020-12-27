#include "socket.h"

#include <iostream>
#include <stdexcept>

using namespace std;

Socket::Socket(void* context, SocketType socket_type, ConnectionType connection_type, std::string endpoint)
    : socket_type_(socket_type), connection_type_(connection_type), endpoint_(endpoint) {
  socket_ = create_zmq_socket(context, socket_type_);
  switch (connection_type_) {
    case ConnectionType::BIND:
      bind_zmq_socket(socket_, endpoint_);
      break;
    case ConnectionType::CONNECT:
      connect_zmq_socket(socket_, endpoint_);
      break;
    default:
      throw logic_error("Undefined connection type");
  }
}

Socket::~Socket() {
  try {
    switch (connection_type_) {
      case ConnectionType::BIND:
        unbind_zmq_socket(socket_, endpoint_);
        break;
      case ConnectionType::CONNECT:
        disconnect_zmq_socket(socket_, endpoint_);
        break;
    }
    close_zmq_socket(socket_);
  } catch (exception& ex) {
    cerr << "Socket wasn't closed: " << ex.what() << endl;
  }
}

void Socket::send(Message message) {
  if (socket_type_ == SocketType::PUBLISHER) {
    send_zmq_msg(socket_, message);
  } else {
    throw logic_error("SUB socket can't send messages");
  }
}

Message Socket::receive() {
  if (socket_type_ == SocketType::SUBSCRIBER) {
    return get_zmq_msg(socket_);
  } else {
    throw logic_error("PUB socket can't receive messages");
  }
}