#include <iostream>
#include <string>
#include <vector>
#include "server.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
	vector<socket_info> connected_client_sockets;
	vector<thread> socket_threads;

	initialize_winsock();
	socket_info listening_socket = create_listening_socket();

	if (bind(listening_socket.sock, (sockaddr*)& listening_socket.sock_addr, sizeof(listening_socket.sock_addr)) < 0)
	{
		exit_with_err_msg("Binding listening socket failed.");
	}

	listen(listening_socket.sock, SOMAXCONN);
	mutex master_mutex; //mutex and shared data protections will be segmented properly in the future
	thread client_connection_thread = thread(wait_for_clients, ref(connected_client_sockets), ref(socket_threads), ref(listening_socket), ref(master_mutex));
	
	client_connection_thread.join();
	WSACleanup();

	return 0;
}