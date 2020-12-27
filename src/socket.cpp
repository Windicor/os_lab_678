#include "socket.h"

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
  switch (connection_type_) {
    case ConnectionType::BIND:
      unbind_zmq_socket(socket_, endpoint_);
      break;
    case ConnectionType::CONNECT:
      disconnect_zmq_socket(socket_, endpoint_);
      break;
  }
  close_zmq_socket(socket_);
}

void Socket::Send(Message message) {
  send_zmq_msg(socket_, message);
}

Message Socket::Receive() {
  return get_zmq_msg(socket_);
}