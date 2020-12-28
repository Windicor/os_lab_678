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
std::string create_endpoint(EndpointType type, pid_t id);

void bind_zmq_socket(void* socket, std::string endpoint);
void unbind_zmq_socket(void* socket, std::string endpoint);
void connect_zmq_socket(void* socket, std::string endpoint);
void disconnect_zmq_socket(void* socket, std::string endpoint);

enum class CommandType {
  ERROR,
  RETURN,
  CREATE_CHILD
};

struct Message {
  CommandType command = CommandType::ERROR;
  int to_id;
  int value;
  bool go_up = false;

  Message() {}
  Message(CommandType command, int to_id, int value)
      : command(command), to_id(to_id), value(value) {}
};

void send_zmq_msg(void* socket, Message msg);
Message get_zmq_msg(void* socket);