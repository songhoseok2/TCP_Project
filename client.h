#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include "common_lib.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

socket_info create_server_socket();

bool receive_process_result(socket_info& server_socket, const char current_request);

bool send_request(socket_info& server_socket, const char current_request);

void send_message(socket_info& server_socket);

void receive_account_balance(socket_info& server_socket);

int get_account_number();

void send_account_number(socket_info& server_socket);

void client_requests(socket_info& server_socket);

#endif