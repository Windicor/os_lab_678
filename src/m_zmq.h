#pragma once

#include <string>

void* create_zmq_context();
void destroy_zmq_context(void* context);

enum class SocketType {
  PUBLISHER,
  SUBSCRIBER
};
void* create_zmq_socket(void* context, SocketType type);
void close_zmq_socket(void* socket);

enum class EndpointType {
  CHILD_PUB,
  PARRENT_PUB,
};
std::string create_endpoint(EndpointType type);

void bind_zmq_socket(void* socket, std::string endpoint);
void unbind_zmq_socket(void* socket, std::string endpoint);
void connect_zmq_socket(void* socket, std::string endpoint);
void disconnect_zmq_socket(void* socket, std::string endpoint);