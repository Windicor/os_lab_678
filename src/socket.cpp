#include "socket.h"

#include <stdexcept>

using namespace std;

Socket::Socket(void* context, SocketType socket_type, EndpointType endpoint_type, ConnectionType connection_type)
    : connection_type_(connection_type) {
  socket_ = create_zmq_socket(context, socket_type);
  endpoint_ = create_endpoint(endpoint_type);
  switch (connection_type) {
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