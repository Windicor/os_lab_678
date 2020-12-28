#include <signal.h>

#include <iostream>
#include <string>

#include "client.h"

using namespace std;

const int ERR_TERMINATED = 1;

Client* client_ptr = nullptr;
void TerminateByUser(int) {
  if (client_ptr != nullptr) {
    client_ptr->~Client();
  }
  cerr << to_string(getpid()) + " Terminated by user"s << endl;
  exit(0);
}

void process_msg(Client& client, Message msg) {
  switch (msg.command) {
    case CommandType::ERROR:
      throw runtime_error("Error message recieved");
    case CommandType::RETURN:
      cout << to_string(getpid()) + " Client message: "s << static_cast<int>(msg.command) << " " << msg.to_id << " " << msg.value << endl;
      client.send_up(msg);
      break;
    case CommandType::CREATE_CHILD:
      cout << to_string(getpid()) + " Client message: "s << static_cast<int>(msg.command) << " " << msg.to_id << " " << msg.value << endl;
      client.add_child(msg.value);
      break;
    default:
      throw logic_error("Not implemented message command");
  }
}

int main(int argc, char const* argv[]) {
  if (argc != 3) {
    cerr << argc;
    cerr << "USAGE: " << argv[0] << " <id> <parrent_pub_endpoint" << endl;
  }

  try {
    if (signal(SIGINT, TerminateByUser) == SIG_ERR) {
      throw runtime_error("Can't set SIGINT signal");
    }
    if (signal(SIGSEGV, TerminateByUser) == SIG_ERR) {
      throw runtime_error("Can't set SIGSEGV signal");
    }

    Client client(stoi(argv[1]), string(argv[2]));
    client_ptr = &client;
    cerr << to_string(getpid()) + " Client is started correctly"s << endl;

    while (true) {
      Message msg = client.receive();
      if (msg.to_id != client.id()) {
        if (msg.go_up) {
          client.send_up(msg);
        } else {
          client.send_down(msg);
        }
        continue;
      }
      process_msg(client, msg);
    }

  } catch (exception& ex) {
    cerr << to_string(getpid()) + " Client exception: "s << ex.what() << "\nTerminated by exception" << endl;
    exit(ERR_TERMINATED);
  }
  cerr << to_string(getpid()) + " Client is finished correctly"s << endl;
  return 0;
}