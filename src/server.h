#pragma once

#include <unistd.h>

class Server {
 public:
  Server();
  ~Server();
  pid_t pid() const;

 private:
  pid_t pid_;
  void* context_;
};