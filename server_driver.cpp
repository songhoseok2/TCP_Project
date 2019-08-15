#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "server.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void read_in_balance_data(double memory[NUMMEMORY])
{
	ifstream infile("balance_data.txt");
	double current_balance;
	for (int i = 0; infile >> current_balance; ++i) { memory[i] = current_balance; }
	infile.close();
}

void update_balance_data(double memory[NUMMEMORY])
{
	ofstream outfile("balance_data.txt");
	for (int i = 0; memory[i] >= 0; ++i)
	{
		outfile << fixed << setprecision(2) << memory[i] << endl;
	}
	outfile.close();
}

void initialize_cache_and_mem(double memory[NUMMEMORY], account_cache_set cache[CACHENUMOFSETS])
{
	fill(memory, memory + NUMMEMORY, -1);
	for (int i = 0; i < CACHENUMOFSETS; ++i)
	{
		for (int k = 0; k < SETBLOCKSIZE; ++k)
		{
			fill(cache[i].tag_blocks[k].cache_lines, cache[i].tag_blocks[k].cache_lines + TAGBLOCKSIZE, -1);
		}
	}
}

int main()
{
	vector<socket_info> connected_client_sockets;
	vector<thread> socket_threads;
	double memory[NUMMEMORY];
	account_cache_set cache[CACHENUMOFSETS];
	initialize_cache_and_mem(memory, cache);

	read_in_balance_data(memory);
	initialize_winsock();
	socket_info listening_socket = create_listening_socket();

	if (bind(listening_socket.sock, (sockaddr*)& listening_socket.sock_addr, sizeof(listening_socket.sock_addr)) < 0)
	{
		exit_with_err_msg("Binding listening socket failed.");
	}

	listen(listening_socket.sock, SOMAXCONN);
	mutex master_mutex; //mutex and shared data protections will be segmented and implemented properly in the future
	thread client_connection_thread = thread(wait_for_clients,
		ref(connected_client_sockets),
		ref(socket_threads),
		ref(listening_socket),
		ref(master_mutex),
		memory,
		cache);

	client_connection_thread.join();
	WSACleanup();
	update_balance_data(memory);
	return 0;
}