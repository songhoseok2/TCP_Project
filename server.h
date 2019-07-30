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

void get_client_connection_info(socket_info& client_socket);

void accept_requests(int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex);

void wait_for_clients(vector<socket_info>& connected_client_sockets, vector<thread>& socket_threads, socket_info& listening_socket, mutex& master_mutex);

void temp_action(char& current_request);

void process_requests(char& current_request, mutex& master_mutex);

#endif