#pragma once

#include <unistd.h>

#include <memory>

#include "socket.h"
#include "tree.h"

class Server {
 public:
  Server();
  ~Server();

  pid_t pid() const;
  Message last_message() const;

  void send(Message message);
  Message receive();

  bool check(int id);
  void create_child_cmd(int id, int parrent_id);

  friend void* receive_msg_loop(void* serv_arg);

 private:
  pid_t pid_;
  void* context_ = nullptr;
  std::unique_ptr<Socket> publiser_;
  std::unique_ptr<Socket> subscriber_;

  pthread_t receive_msg_loop_id;
  bool terminated_ = false;

  Message last_message_;
  IdTree tree_;
};