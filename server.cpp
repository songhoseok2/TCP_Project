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
	inet_ntop(AF_INET, &ipAddr, client_socket.IP_address, INET_ADDRSTRLEN);
	client_socket.port_num = ntohs(client_socket.sock_addr.sin_port);
}

//receive request from client
//send request acceptance result to client
//process request
//send request result to client

void accept_requests(int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex)
{
	char request_buff[2];
	for (ZeroMemory(request_buff, 2); true; ZeroMemory(request_buff, 2))
	{
		//wait for client to send request
		int bytes_received = recv(connected_client_sockets[thread_number].sock, request_buff, sizeof(request_buff)+10, 0);
		if (bytes_received == SOCKET_ERROR)
		{
			cout << "ERROR in receiving request from client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
			cout << "ERROR number: " << WSAGetLastError() << endl;
			break;
		}
		else if (bytes_received == 0)
		{
			cout << "Client " << connected_client_sockets[thread_number].IP_address << " has disconnected from port " << connected_client_sockets[thread_number].port_num << endl;
			break;
		}

		cout << connected_client_sockets[thread_number].IP_address << ", thread_number " << thread_number << " requested " << request_buff << "." << endl;
		
		//echo request acceptance result to client
		char request_acceptance_result_buff[2];
		request_acceptance_result_buff[0] = 'y';
		request_acceptance_result_buff[1] = '\0';
		int bytes_sent = send(connected_client_sockets[thread_number].sock, request_acceptance_result_buff, sizeof(request_acceptance_result_buff) , 0);
		if (bytes_sent == SOCKET_ERROR)
		{
			cout << "ERROR in sending request acceptance result to client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
			cout << "ERROR number: " << WSAGetLastError() << endl;
			break;
		}
		else if (bytes_sent == 0)
		{
			cout << "Client " << connected_client_sockets[thread_number].IP_address << " has disconnected from port " << connected_client_sockets[thread_number].port_num << endl;
			break;
		}

		char current_request = request_buff[0];
		cout << current_request << " request from " << connected_client_sockets[thread_number].IP_address << " accepted. Processing." << endl;
		process_requests(current_request, thread_number, connected_client_sockets, master_mutex);
		//process results will be handled in each process' function
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
		cout << "Client IP address " << client_socket.IP_address << " has connected to port " << client_socket.port_num << "." << endl;

		master_mutex.lock();
		connected_client_sockets.push_back(client_socket);
		socket_threads.push_back(thread(accept_requests, thread_number++, ref(connected_client_sockets), ref(master_mutex)));
		master_mutex.unlock();
	}
}

void send_process_result(const char current_request, int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex)
{
	char request_result_buff[2];
	switch (current_request)
	{
	case 'm':
		request_result_buff[0] = 'p'; //processed properly
		request_result_buff[1] = '\0';
		break;
	}

	int bytes_sent = send(connected_client_sockets[thread_number].sock, request_result_buff, size(request_result_buff) , 0); //2 cuz one char + terminating null
	if (bytes_sent == SOCKET_ERROR)
	{
		cout << "ERROR in sending request acceptance result to client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
		cout << "ERROR number: " << WSAGetLastError() << endl;
		return;
	}
	else if (bytes_sent == 0)
	{
		cout << "Client " << connected_client_sockets[thread_number].IP_address << " has disconnected from port " << connected_client_sockets[thread_number].port_num << endl;
		return;
	}
	
	cout << "Sent result " << request_result_buff << " to Client " << connected_client_sockets[thread_number].IP_address << "." << endl;
}

void receive_message(int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex)
{
	char msg_buff[4096];

	int bytes_received = recv(connected_client_sockets[thread_number].sock, msg_buff, sizeof(msg_buff), 0);
	if (bytes_received == SOCKET_ERROR)
	{
		cout << "ERROR in receiving request from client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
		cout << "ERROR number: " << WSAGetLastError() << endl;
		return;
	}
	else if (bytes_received == 0) //becomes 4096
	{
		cout << "Client " << connected_client_sockets[thread_number].IP_address << " has disconnected from port " << connected_client_sockets[thread_number].port_num << endl;
		return;
	}

	cout << connected_client_sockets[thread_number].IP_address << ", thread_number " << thread_number << " messaged: \"" << msg_buff << "\"" << endl;
}

void process_requests(const char current_request, int thread_number, vector<socket_info>& connected_client_sockets, mutex& master_mutex)
{
	master_mutex.lock();
	if (current_request == 'm')
	{
		receive_message(thread_number, connected_client_sockets, master_mutex);
		send_process_result(current_request, thread_number, connected_client_sockets, master_mutex);
	}
	master_mutex.unlock();
}