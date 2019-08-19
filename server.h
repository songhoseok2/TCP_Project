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

void client_disconnection_message(socket_info* const client_socket);

void set_client_connection_info(socket_info* client_socket);

void accept_requests(vector<socket_info>& connected_client_sockets,
	vector<thread>& socket_threads,
	socket_info* client_socket,
	mutex& master_mutex, double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void wait_for_clients(vector<socket_info>& connected_client_sockets,
	vector<thread>& socket_threads,
	socket_info& listening_socket,
	mutex& master_mutex,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void send_process_result(const char request_result_buff[2],
	socket_info* client_socket,
	mutex& master_mutex);

int receive_account_number(mutex& master_mutex,
	socket_info* client_socket);

double receive_new_balance(mutex& master_mutex,
	socket_info* client_socket);

char receive_message(socket_info* client_socket,
	mutex& master_mutex);

void process_request_m(socket_info* client_socket,
	mutex& master_mutex);

double read_account(const int account_number,
	mutex& master_mutex,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void process_request_r(socket_info* client_socket,
	mutex& master_mutex,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void process_request_u(socket_info* client_socket,
	mutex& master_mutex,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void process_requests(const char current_request,
	socket_info* client_socket,
	mutex& master_mutex,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

#endif