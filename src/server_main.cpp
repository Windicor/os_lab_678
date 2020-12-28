#include <signal.h>

#include <iostream>
#include <string>

#include "server.h"
#include "tree.h"

using namespace std;

const int ERR_TERMINATED = 1;
const double MESSAGE_WAITING_TIME = 1;

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
    int to_id, value;
    cin >> to_id >> value;
    Message msg(CommandType::RETURN, to_id, value);
    server.send(msg);
    sleep(MESSAGE_WAITING_TIME);
    if (server.last_message() == msg) {
      cout << "OK" << endl;
    } else {
      cout << "NOT_OK" << endl;
    }
  } else if (cmd == "create") {
    int id, parrent_id;
    cin >> id >> parrent_id;
    server.send(Message(CommandType::CREATE_CHILD, parrent_id, id));
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
