#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <mutex>
#include <thread>
#include "common_lib.h"
#include <WS2tcpip.h>

using namespace std;

socket_info create_listening_socket();

void client_disconnection_message(const char IP_address[INET_ADDRSTRLEN], const int port_num);

void set_client_connection_info(socket_info& client_socket);

void accept_requests(const int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex);

void wait_for_clients(vector<socket_info>& connected_client_sockets, vector<thread>& socket_threads, socket_info& listening_socket, mutex& master_mutex);

void send_process_result(const char request_result_buff[2], const int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex);

void receive_message(const int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex);

bank_account read_account(const string account_holder, char& process_result);

void send_account_balance();

void update_account(const string account_holder, const double new_balance, char& process_result);

string get_account_holder_name();

void process_requests(const char current_request, const int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex);

#endif