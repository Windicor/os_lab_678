#pragma once

#define SERVER_SOCKET_PUB "ipc:///tmp/server_pub_"
#define CLIENT_CHILD_PUB_PATTERN "ipc://tmp/client_child_pub_"
#define CLIENT_PARRENT_PUB_PATTERN "ipc://tmp/client_parrent_pub_"

void* create_zmq_context();
void destroy_zmq_context(void* context);