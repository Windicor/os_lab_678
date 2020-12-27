#include "m_zmq.h"

#include <unistd.h>
#include <zmq.h>

#include <iostream>

using namespace std;

void* create_zmq_context() {
  void* context = zmq_ctx_new();
  if (context == NULL) {
    throw runtime_error("Can't create new context. pid:" + to_string(getpid()));
  }
  return context;
}

void destroy_zmq_context(void* context) {
  if (zmq_ctx_destroy(context) != 0) {
    throw runtime_error("Can't destroy context. pid:" + to_string(getpid()));
  }
}

int get_zmq_socket_type(SocketType type) {
  switch (type) {
    case SocketType::PUBLISHER:
      return ZMQ_PUB;
    case SocketType::SUBSCRIBER:
      return ZMQ_SUB;
    default:
      throw logic_error("Undefined socket type");
  }
}

void* create_zmq_socket(void* context, SocketType type) {
  int zmq_type = get_zmq_socket_type(type);
  void* socket = zmq_socket(context, zmq_type);
  if (socket == NULL) {
    throw runtime_error("Can't create socket");
  }
  if (zmq_type == ZMQ_SUB) {
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, 0, 0);
  }
  return socket;
}

void close_zmq_socket(void* socket) {
  if (zmq_close(socket) != 0) {
    throw runtime_error("Can't close socket");
  }
}

string create_endpoint(EndpointType type) {
  string endpoint;
  switch (type) {
    case EndpointType::PARRENT_PUB:
      return "ipc://tmp/parrent_pub_"s + to_string(getpid());
    case EndpointType::CHILD_PUB:
      return "ipc://tmp/child_pub_"s + to_string(getpid());
    default:
      throw logic_error("Undefined endpoint type");
  }
}

void bind_zmq_socket(void* socket, string endpoint) {
  if (zmq_bind(socket, endpoint.data()) != 0) {
    throw runtime_error("Can't bind socket");
  }
}

void unbind_zmq_socket(void* socket, string endpoint) {
  if (zmq_unbind(socket, endpoint.data()) != 0) {
    throw runtime_error("Can't unbind socket");
  }
}

void connect_zmq_socket(void* socket, string endpoint) {
  if (zmq_connect(socket, endpoint.data()) != 0) {
    throw runtime_error("Can't connect socket");
  }
}

void disconnect_zmq_socket(void* socket, string endpoint) {
  if (zmq_disconnect(socket, endpoint.data()) != 0) {
    throw runtime_error("Can't disconnect socket");
  }
}
