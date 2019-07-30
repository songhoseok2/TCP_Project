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

void print_connection_info(sockaddr_in& client)
{
	struct sockaddr_in* pV4Addr = (struct sockaddr_in*) & client;
	struct in_addr ipAddr = pV4Addr->sin_addr;
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
	int port = ntohs(client.sin_port);

	cout << "-[Client IP:" << str << ", Connected on PORT:" << port << "]" << std::endl;
}

void accept_requests(socket_info& client_socket, mutex& master_mutex)
{
	//loop to accept and echo message back to client
	char buff[4096];
	for (ZeroMemory(buff, 4096); true; ZeroMemory(buff, 4096))
	{
		//wait for client to send data
		int bytes_received = recv(client_socket.sock, buff, 4096, 0);
		if (bytes_received == SOCKET_ERROR)
		{
			cout << "ERROR in recv(). Exiting." << endl;
			break;
		}
		else if (bytes_received == 0)
		{
			cout << "Client disconnected." << endl;
			break;
		}

		cout << "Received: " << buff << endl;

		//echo message back to client
		int bytes_sent = send(client_socket.sock, buff, bytes_received + 1, 0); //+1 cuz of the terminating \0
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

		cout << "A client has connected.";
		print_connection_info(client_socket.sock_addr);

		char host[NI_MAXHOST];			//Client's remote name
		char service[NI_MAXSERV];		//Service (i.e. port) the client is connected on

		ZeroMemory(host, NI_MAXHOST);	//memset(host, 0, NI_MAXHOST)
		ZeroMemory(service, NI_MAXSERV);


		/*
		cout << host << "connected on port ";
		if (getnameinfo((sockaddr*)& client_socket.sock, client_size, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		{
			cout << service << ". ";
			cout << "getnameinfo successful.";
		}
		else
		{
			inet_ntop(AF_INET, &client_socket.sock_addr.sin_addr, host, NI_MAXHOST);
			cout << ntohs(client_socket.sock_addr.sin_port) << ". ";
			cout << "getnameinfo failed.";
		}
		cout << endl;
		*/

		master_mutex.lock();
		connected_client_sockets.push_back(client_socket);
		socket_threads.push_back(thread(accept_requests, ref(connected_client_sockets.back()), ref(master_mutex)));
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