#pragma once

#include <unistd.h>

#include <memory>
#include <string>

#include "socket.h"

class Client {
 public:
  Client(int id, std::string parrent_endpoint);
  ~Client();

  pid_t pid() const;

  void send_up(Message message);
  void send_down(Message message);
  Message receive();

 private:
  int id_;
  pid_t pid_;
  void* context_ = nullptr;
  std::unique_ptr<Socket> child_publiser_;
  std::unique_ptr<Socket> parrent_publiser_;
  std::unique_ptr<Socket> subscriber_;

  bool terminated_ = false;
};