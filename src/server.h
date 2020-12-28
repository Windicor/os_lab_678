#pragma once

#include <unistd.h>

#include <memory>
#include <unordered_map>

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
  void remove_child_cmd(int id);
  void exec_cmd(int id, CommandType type);
  void heartbit_cmd(int time);
  void print_tree();

  friend void* second_thread(void* serv_arg);

 private:
  pid_t pid_;
  void* context_ = nullptr;
  std::unique_ptr<Socket> publiser_;
  std::unique_ptr<Socket> subscriber_;

  pthread_t receive_msg_loop_id;
  bool terminated_ = false;

  Message last_message_;
  IdTree tree_;
  std::unordered_map<int, bool> map_for_check_;
};