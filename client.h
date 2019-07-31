#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include "common_lib.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

socket_info create_server_socket();

void receive_request_result(socket_info& server_socket, const char current_request);

bool send_request(socket_info& server_socket, const char current_request);

void send_message(socket_info& server_socket);

void client_requests(socket_info& server_socket);

#endif