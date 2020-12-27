#pragma once

#include <string>

#include "m_zmq.h"

enum class ConnectionType {
  BIND,
  CONNECT
};

class Socket {
 public:
  Socket(void* context, SocketType socket_type, ConnectionType connection_type, std::string endpoint);
  ~Socket();

  void send(Message message);
  Message receive();

  std::string endpoint() const;

 private:
  void* socket_;
  SocketType socket_type_;
  ConnectionType connection_type_;
  std::string endpoint_;
};