#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <iomanip>
#include <string>
#include "common_lib.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

socket_info create_server_socket();

void connect_to_server(socket_info& server_socket);

void server_disconnection(socket_info& server_socket, const char IP_address[INET_ADDRSTRLEN], const int port_num);

bool receive_process_result(socket_info& server_socket, const char current_request);

int get_account_number();

bool is_valid_balance(const string input);

double get_new_balance();

bool send_request(socket_info& server_socket, const char current_request);

void send_message(socket_info& server_socket);

void receive_account_balance(socket_info& server_socket);

void send_account_number(socket_info& server_socket);

void send_new_balance(socket_info& server_socket);

void client_requests(socket_info& server_socket);

#endif