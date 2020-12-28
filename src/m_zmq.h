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
  CREATE_CHILD,
  REMOVE_CHILD,
  TIMER_TIME,
  TIMER_START,
  TIMER_STOP,
  HEARTBIT
};

struct Message {
  CommandType command = CommandType::ERROR;
  int to_id;
  int value;
  bool go_up = false;
  int uniq_num;

  Message();
  Message(CommandType command_a, int to_id_a, int value_a);
};

bool operator==(const Message& lhs, const Message& rhs);

void send_zmq_msg(void* socket, Message msg);
Message get_zmq_msg(void* socket);