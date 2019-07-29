#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include "common_lib.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

socket_info create_client_socket();

#endif