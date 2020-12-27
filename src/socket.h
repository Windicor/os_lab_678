#pragma once

#include <string>

#include "m_zmq.h"

enum class ConnectionType {
  BIND,
  CONNECT
};

class Socket {
 public:
  Socket(void* context, SocketType socket_type, EndpointType endpoint_type, ConnectionType connection_type);
  ~Socket();

 private:
  void* socket_;
  std::string endpoint_;
  ConnectionType connection_type_;
};