#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <algorithm>
#include "common_lib.h"
#include <WS2tcpip.h>

using namespace std;

socket_info create_listening_socket();

socket_info connect_to_a_client(socket_info listening_socket);

void temporary_action(socket_info& client_socket);

#endif