#pragma once

#include <unistd.h>

#include <memory>

#include "socket.h"

class Server {
 public:
  Server();
  ~Server();

  pid_t pid() const;

 private:
  pid_t pid_;
  void* context_ = nullptr;
  std::unique_ptr<Socket> publiser_;
  std::unique_ptr<Socket> subscriber_;
};