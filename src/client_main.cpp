#include <iostream>
#include <string>

#include "client.h"

using namespace std;

const int ERR_TERMINATED = 1;

int main(int argc, char const* argv[]) {
  if (argc != 3) {
    cerr << argc;
    cerr << "USAGE: " << argv[0] << " <id> <parrent_pub_endpoint" << endl;
  }

  try {
    Client client(stoi(argv[1]), string(argv[2]));
    cerr << to_string(getpid()) + " Client is started correctly"s << endl;

    while (true) {
      Message msg = client.receive();
      cout << to_string(getpid()) + " Client message: "s << static_cast<int>(msg.command) << " " << msg.to_id << " " << msg.value << endl;
      client.send_up(msg);
    }

  } catch (exception& ex) {
    cerr << to_string(getpid()) + " Client exception: "s << ex.what() << "\nTerminated by exception" << endl;
    exit(ERR_TERMINATED);
  }
  cerr << to_string(getpid()) + " Client is ended correctly"s << endl;
  return 0;
}