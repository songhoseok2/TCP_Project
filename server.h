#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include "common_lib.h"
#include "cache_system.h"
#include <WS2tcpip.h>

using namespace std;

//this would be better as map so that address of remaining sockets won't be changed when
// a thread completes and closes & erases its socket
//much easier for mutual exclusion
extern map<thread::id, socket_info> connected_client_sockets;

socket_info create_listening_socket();

void client_disconnection_message(socket_info* const client_socket);

void set_client_connection_info(socket_info* client_socket);

void accept_requests(vector<thread>& threads_vec, 
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void wait_for_clients(vector<thread>& threads_vec,
	socket_info& listening_socket,
	condition_variable& cv,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void send_process_result(const char request_result_buff[2],
	socket_info* client_socket);

int receive_account_number(socket_info* client_socket);

double receive_new_balance(socket_info* client_socket);

char receive_message(socket_info* client_socket);

void process_request_m(socket_info* client_socket);

double read_account(const int account_number,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void process_request_r(socket_info* client_socket,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void process_request_u(socket_info* client_socket,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void process_requests(const char current_request,
	socket_info* client_socket,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

#endif