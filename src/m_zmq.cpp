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
