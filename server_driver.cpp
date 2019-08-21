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

void wait_for_termination_input(condition_variable& cv)
{
	mutex temp_mutex;
	unique_lock<mutex> stuff(temp_mutex);
	cv.wait(stuff);
	//wait because this message should appear AFTER wait_for_clients' message display first
	cout << "Enter ESC to terminate this server." << endl;

	while (true)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			if (connected_client_sockets.empty()) { return; }
			else
			{
				for (map<thread::id, socket_info>::iterator it = connected_client_sockets.begin(); it != connected_client_sockets.end(); ++it)
				{
					cout << "IP Adress: " << it->second.IP_address << " on port: " << it->second.port_num << endl;
				}
				cout << "There are " << connected_client_sockets.size() << " socket(s) still connected to this server." << endl;
				cout << "It is recommended that you terminate the server after all clients finish their operations and disconnect from this server." << endl;
				cout << "Do you still wish to terminate this server? y/n: " << endl;
				string answer;
				while (getline(cin, answer))
				{
					if (answer == "y")
					{
						for (map<thread::id, socket_info>::iterator it = connected_client_sockets.begin(); it != connected_client_sockets.end(); ++it)
						{
							closesocket(it->second.sock);
						}
						return;
					}
					else if (answer == "n") { break; }
					else { cout << "Please answer in y or n: "; }
				}
			}
		}
	}
}

int main()
{
	vector<thread> threads_vec;
	double memory[NUMMEMORY];
	account_cache_set cache[CACHENUMOFSETS];
	initialize_cache_and_mem(memory, cache);

	read_in_balance_data(memory);
	initialize_winsock();
	socket_info listening_socket = create_listening_socket();

	//if using namespace std AND <functional> library is included this could cause error
	//specify to use bind from global namespace.
	if (::bind(listening_socket.sock, (sockaddr*)& listening_socket.sock_addr, sizeof(listening_socket.sock_addr)) < 0)
	{
		exit_with_err_msg("Binding listening socket failed.");
	}

	listen(listening_socket.sock, SOMAXCONN);
	
	condition_variable cv;
	thread client_connection_thread(wait_for_clients,
		ref(threads_vec),
		ref(listening_socket),
		ref(cv),
		memory,
		cache);

	thread termination_thread(wait_for_termination_input,
		ref(cv));
	termination_thread.join();

	WSACleanup();
	update_balance_data(memory);
	return 0;
}