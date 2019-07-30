#include "common_lib.h"
#include "server.h"
#include <WS2tcpip.h>

using namespace std;

socket_info create_listening_socket()
{
	socket_info listening_socket;

	listening_socket.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_socket.sock == INVALID_SOCKET)
	{
		exit_with_err_msg("listen_socket creation failed. Error #" + to_string(WSAGetLastError()) + ".");
	}

	cout << "Initializing listening socket. ";
	int listen_port_num = get_port_number();

	//setup the host_addr structure for use in bind call
	listening_socket.sock_addr.sin_family = AF_INET;				//server byte order
	listening_socket.sock_addr.sin_addr.S_un.S_addr = INADDR_ANY;	//automatically be filled with current host's IP address
	listening_socket.sock_addr.sin_port = htons(listen_port_num);	//convert short integer value for port must be converted into network byte order
	
	return listening_socket;
}

void get_client_connection_info(socket_info &client_socket)
{
	struct sockaddr_in* pV4Addr = (struct sockaddr_in*) & client_socket.sock_addr;
	struct in_addr ipAddr = pV4Addr->sin_addr;
	inet_ntop(AF_INET, &ipAddr, client_socket.client_IP_address, INET_ADDRSTRLEN);
	client_socket.client_port_num = ntohs(client_socket.sock_addr.sin_port);
}

void accept_requests(int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex)
{
	//loop to accept and echo message back to client
	char buff[4096];
	for (ZeroMemory(buff, 4096); true; ZeroMemory(buff, 4096))
	{
		//wait for client to send data
		int bytes_received = recv(connected_client_sockets[thread_number].sock, buff, 4096, 0);
		if (bytes_received == SOCKET_ERROR)
		{
			cout << "ERROR in recv(). Exiting." << endl;
			cout << "ERROR number: " << WSAGetLastError() << endl;
			break;
		}
		else if (bytes_received == 0)
		{
			cout << "Client IP address " << connected_client_sockets[thread_number].client_IP_address << " has disconnected from port " << connected_client_sockets[thread_number].client_port_num << endl;
			break;
		}

		cout << "Received: " << buff << " in thread number: " << thread_number << endl;
		
		//echo message back to client
		int bytes_sent = send(connected_client_sockets[thread_number].sock, buff, bytes_received + 1, 0); //+1 cuz of the terminating \0
		if (bytes_sent == SOCKET_ERROR)
		{
			cout << "ERROR in send(). Exiting." << endl;
			break;
		}
		else if (bytes_sent == 0)
		{
			cout << "Client disconnected." << endl;
			break;
		}
	}
}

//code to wait for and connect to clients real time
void wait_for_clients(vector<socket_info>& connected_client_sockets, vector<thread> &socket_threads, socket_info& listening_socket, mutex &master_mutex)
{
	int thread_number = 0;
	while (true)
	{
		//Wait for connection
		socket_info client_socket;
		int client_size = sizeof(client_socket.sock_addr);
		//cout << "Waiting to connect to a client...";
		client_socket.sock = accept(listening_socket.sock, (sockaddr*)& client_socket.sock_addr, &client_size);

		if (client_socket.sock == INVALID_SOCKET)
		{
			exit_with_err_msg("client_socket creation failed.");
		}

		get_client_connection_info(client_socket);
		cout << "Client IP address " << client_socket.client_IP_address << " has connected to port " << client_socket.client_port_num << "." << endl;

		master_mutex.lock();
		connected_client_sockets.push_back(client_socket);
		socket_threads.push_back(thread(accept_requests, thread_number++, ref(connected_client_sockets), ref(master_mutex)));
		master_mutex.unlock();
	}
}

void temp_action(char& current_request)
{
	//do stuff



	current_request = 'n';
}

void process_requests(char& current_request, mutex& master_mutex)
{
	while (true)
	{
		if (current_request != 'n')
		{
			master_mutex.lock();
			temp_action(current_request);
			master_mutex.unlock();
		}
	}
}