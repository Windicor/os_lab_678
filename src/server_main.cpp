#include <signal.h>

#include <iostream>
#include <string>

#include "server.h"

using namespace std;

const int ERR_TERMINATED = 1;

Server* server_ptr = nullptr;
void TerminateByUser(int) {
  if (server_ptr != nullptr) {
    server_ptr->~Server();
  }
  cerr << to_string(getpid()) + " Terminated by user"s << endl;
  exit(0);
}

void process_cmd(Server& server, string cmd) {
  if (cmd == "check") {
    int id;
    cin >> id;
    if (server.check(id)) {
      cout << "OK" << endl;
    } else {
      cout << "NOT_OK" << endl;
    }
  } else if (cmd == "create") {
    int id, parrent_id;
    cin >> id >> parrent_id;
    server.create_child_cmd(id, parrent_id);
  } else if (cmd == "remove") {
    int id;
    cin >> id;
    server.remove_child_cmd(id);
  } else if (cmd == "exec") {
    int id;
    string sub_cmd;
    cin >> id >> sub_cmd;
    CommandType type;
    if (sub_cmd == "time") {
      type = CommandType::TIMER_TIME;
    } else if (sub_cmd == "start") {
      type = CommandType::TIMER_START;
    } else if (sub_cmd == "stop") {
      type = CommandType::TIMER_STOP;
    } else {
      cout << "Incorrect subcommand" << endl;
      return;
    }
    server.exec_cmd(id, type);
  } else if (cmd == "heartbit") {
    int time;
    cin >> time;
    server.heartbit_cmd(time);
  } else if (cmd == "print_tree") {
    server.print_tree();
  } else {
    cout << "It's not a command" << endl;
  }
}

int main() {
  try {
    if (signal(SIGINT, TerminateByUser) == SIG_ERR) {
      throw runtime_error("Can't set SIGINT signal");
    }
    if (signal(SIGSEGV, TerminateByUser) == SIG_ERR) {
      throw runtime_error("Can't set SIGSEGV signal");
    }

    Server server;
    server_ptr = &server;
    cerr << to_string(getpid()) + " Server is started correctly"s << endl;

    string cmd;
    while (cin >> cmd) {
      process_cmd(server, cmd);
    }

  } catch (exception& ex) {
    cerr << to_string(getpid()) + " Server exception: "s << ex.what() << "\nTerminated by exception" << endl;
    exit(ERR_TERMINATED);
  }
  cerr << to_string(getpid()) + " Server is finished correctly"s << endl;
  return 0;
}
