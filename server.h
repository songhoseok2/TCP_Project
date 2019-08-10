#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <mutex>
#include <thread>
#include "common_lib.h"
#include "cache_system.h"
#include <WS2tcpip.h>

using namespace std;

socket_info create_listening_socket();

void client_disconnection_message(const char IP_address[INET_ADDRSTRLEN], const int port_num);

void set_client_connection_info(socket_info& client_socket);

void accept_requests(	const int thread_number,
						vector<socket_info>& connected_client_sockets,
						mutex& master_mutex, double memory[NUMMEMORY],
						account_cache_set cache[CACHENUMOFSETS]);

void wait_for_clients(	vector<socket_info>& connected_client_sockets,
						vector<thread>& socket_threads,
						socket_info& listening_socket,
						mutex& master_mutex,
						double memory[NUMMEMORY],
						account_cache_set cache[CACHENUMOFSETS]);

void send_process_result(	const char request_result_buff[2],
							const int thread_number,
							vector<socket_info>& connected_client_sockets,
							mutex& master_mutex);

void receive_message(	const int thread_number,
						vector<socket_info>& connected_client_sockets,
						mutex& master_mutex);

void process_request_m(	const int thread_number,
						vector<socket_info>& connected_client_sockets,
						mutex& master_mutex);

void process_request_r(	const int thread_number,
						vector<socket_info>& connected_client_sockets,
						mutex& master_mutex,
						double memory[NUMMEMORY],
						account_cache_set cache[CACHENUMOFSETS]);

double read_account(const int account_number,
					mutex& master_mutex,
					double memory[NUMMEMORY],
					account_cache_set cache[CACHENUMOFSETS]);

int get_account_number();

void process_requests(	const char current_request,
						const int thread_number, 
						vector<socket_info>& connected_client_sockets, 
						mutex& master_mutex, 
						double memory[NUMMEMORY], 
						account_cache_set cache[CACHENUMOFSETS]);

#endif