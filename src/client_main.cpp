#include <signal.h>

#include <iostream>
#include <string>

#include "client.h"

using namespace std;

const int ERR_TERMINATED = 1;
const int UNIVERSAL_MESSAGE_ID = -256;

Client* client_ptr = nullptr;
void TerminateByUser(int) {
  if (client_ptr != nullptr) {
    client_ptr->~Client();
  }
  cerr << to_string(getpid()) + " Terminated by user"s << endl;
  exit(0);
}

void process_msg(Client& client, const Message msg) {
  cerr << to_string(getpid()) + " Client message: "s << static_cast<int>(msg.command) << " " << msg.to_id << " " << msg.value << endl;
  switch (msg.command) {
    case CommandType::ERROR:
      throw runtime_error("Error message recieved");
    case CommandType::RETURN:
      client.send_up(msg);
      break;
    case CommandType::CREATE_CHILD:
      client.add_child(msg.value);
      break;
    case CommandType::REMOVE_CHILD: {
      if (msg.to_id != UNIVERSAL_MESSAGE_ID) {
        client.send_up(msg);
      }
      Message tmp = msg;
      tmp.to_id = UNIVERSAL_MESSAGE_ID;
      client.send_down(tmp);
      TerminateByUser(0);
      break;
    }
    case CommandType::TIMER_START:
      client.start_timer();
      client.send_up(msg);
      break;
    case CommandType::TIMER_STOP:
      client.stop_timer();
      client.send_up(msg);
      break;
    case CommandType::TIMER_TIME: {
      int val = client.get_time();
      client.send_up(Message(CommandType::TIMER_TIME, client.id(), val));
      break;
    }
    case CommandType::HEARTBIT:
      client.send_down(msg);
      client.heartbit(msg.value);
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
      if (msg.to_id != client.id() && msg.to_id != UNIVERSAL_MESSAGE_ID) {
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