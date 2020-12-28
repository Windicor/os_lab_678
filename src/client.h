#pragma once

#include <unistd.h>

#include <chrono>
#include <memory>
#include <string>

#include "socket.h"

class Client {
 public:
  Client(int id, std::string parrent_endpoint);
  ~Client();

  int id() const;
  pid_t pid() const;

  void send_up(Message message);
  void send_down(Message message);
  Message receive();

  void start_timer() {
    is_timer_started = true;
    start_ = std::chrono::steady_clock::now();
  }

  void stop_timer() {
    is_timer_started = false;
    finish_ = std::chrono::steady_clock::now();
  }

  int get_time() {
    if (is_timer_started) {
      finish_ = std::chrono::steady_clock::now();
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(finish_ - start_).count();
  }

  void add_child(int id);

 private:
  int id_;
  pid_t pid_;
  void* context_ = nullptr;
  std::unique_ptr<Socket> child_publiser_;
  std::unique_ptr<Socket> parrent_publiser_;
  std::unique_ptr<Socket> subscriber_;

  bool is_timer_started = false;
  std::chrono::steady_clock::time_point start_;
  std::chrono::steady_clock::time_point finish_;

  bool terminated_ = false;
};