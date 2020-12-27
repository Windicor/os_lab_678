#include <signal.h>

#include <iostream>
#include <string>

#include "server.h"
#include "tree.h"

using namespace std;

const int ERR_TERMINATED = 1;
const int ERR_ABORTED = 1;

Server* server_ptr = nullptr;
void TerminateByUser(int) {
  if (server_ptr != nullptr) {
    server_ptr->~Server();
  }
  cerr << "Terminated by user" << endl;
  exit(0);
}

void TerminateByAbort(int) {
  if (server_ptr != nullptr) {
    server_ptr->~Server();
  }
  cerr << "Terminated by abort (check client exec file)" << endl;
  exit(ERR_ABORTED);
}

int main() {
  try {
    if (signal(SIGINT, TerminateByUser) == SIG_ERR) {
      throw runtime_error("Can't set SIGINT signal");
    }
    if (signal(SIGSEGV, TerminateByUser) == SIG_ERR) {
      throw runtime_error("Can't set SIGSEGV signal");
    }
    if (signal(SIGABRT, TerminateByAbort) == SIG_ERR) {
      throw runtime_error("Can't set SIGABRT signal");
    }

    Server server;
    server_ptr = &server;
    cerr << "Server is started correctly" << endl;

    string cmd;
    while (cin >> cmd) {
      cout << cmd << endl;
    }

  } catch (exception& ex) {
    cerr << "Server exception: " << ex.what() << "\nTerminated by exception" << endl;
    exit(ERR_TERMINATED);
  }
  cerr << "Programm is ended correctly" << endl;
  return 0;
}
