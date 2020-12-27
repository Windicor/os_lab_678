#include <signal.h>

#include <iostream>
#include <string>

#include "server.h"
#include "tree.h"

using namespace std;

#define ERR_TERMINATED 1

Server* server_ptr = nullptr;
void Terminate(int) {
  if (server_ptr != nullptr) {
    server_ptr->~Server();
  }
  cerr << "Terminated by user or exception" << endl;
  exit(ERR_TERMINATED);
}

void Terminate() {
  Terminate(0);
}

int main() {
  try {
    if (signal(SIGINT, Terminate) == SIG_ERR) {
      throw runtime_error("Can't set SIGINT signal");
    }
    if (signal(SIGSEGV, Terminate) == SIG_ERR) {
      throw runtime_error("Can't set SIGSEGV signal");
    }

    Server server;
    server_ptr = &server;
    cerr << "Server is started correctly" << endl;

    string cmd;
    while (cin >> cmd) {
      cout << cmd << endl;
    }
  } catch (exception& ex) {
    cerr << "Exctption: " << ex.what() << endl;
    Terminate();
  }
  cerr << "Programm is ended correctly" << endl;
  return 0;
}
