#include <signal.h>

#include <iostream>
#include <string>

#include "server.h"
#include "tree.h"

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

    int cmd;
    while (cin >> cmd) {
      server.send(Message{CommandType::REQ_REP, cmd, cmd});
    }

  } catch (exception& ex) {
    cerr << to_string(getpid()) + " Server exception: "s << ex.what() << "\nTerminated by exception" << endl;
    exit(ERR_TERMINATED);
  }
  cerr << to_string(getpid()) + " Server is ended correctly"s << endl;
  return 0;
}
