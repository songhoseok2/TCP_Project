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
	//char current_request = 'n';

	initialize_winsock();
	socket_info listening_socket = create_listening_socket();

	//Bind ip address and port to a socket
	if (bind(listening_socket.sock, (sockaddr*)& listening_socket.sock_addr, sizeof(listening_socket.sock_addr)) < 0)
	{
		exit_with_err_msg("Binding listening socket failed.");
	}

	//listen() tells the socket to listen to the incoming connections.
	//The listen() function places all incoming connection into a backlog queue
	//until accept() call accepts the connection.
	//Setting the maximum size for the backlog queue to a defined number.
	listen(listening_socket.sock, SOMAXCONN);
	mutex master_mutex; //mutex and shared data protections will be segmented properly in the future
	thread client_connection_thread = thread(wait_for_clients, ref(connected_client_sockets), ref(socket_threads), ref(listening_socket), ref(master_mutex));
	//thread proccess_thread = thread(process_requests, ref(current_request), ref(master_mutex));
	
	client_connection_thread.join();
	//proccess_thread.join();
	WSACleanup();

	return 0;
}