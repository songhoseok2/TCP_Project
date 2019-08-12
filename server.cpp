#include "common_lib.h"
#include "server.h"
#include <WS2tcpip.h>

using namespace std;

void client_disconnection_message(const char IP_address[INET_ADDRSTRLEN], const int port_num)
{
	cout << "Client " << IP_address << " has disconnected from port " << port_num << "." << endl;
}

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
	listening_socket.sock_addr.sin_port = htons(listen_port_num);	//short integer value for port must be converted into network byte order
	
	return listening_socket;
}

void set_client_connection_info(socket_info &client_socket)
{
	struct sockaddr_in* pV4Addr = (struct sockaddr_in*) & client_socket.sock_addr;
	struct in_addr ipAddr = pV4Addr->sin_addr;
	inet_ntop(AF_INET, &ipAddr, client_socket.IP_address, INET_ADDRSTRLEN);
	client_socket.port_num = ntohs(client_socket.sock_addr.sin_port);
}

//order of processing request:
//receive request from client
//send request acceptance result to client
//process request
//send request result to client

void accept_requests(	const int thread_number,
						vector<socket_info>& connected_client_sockets,
						mutex& master_mutex,
						double memory[NUMMEMORY],
						account_cache_set cache[CACHENUMOFSETS])
{
	char request_buff[2];
	for (ZeroMemory(request_buff, 2); true; ZeroMemory(request_buff, 2))
	{
		//receive request from client
		int bytes_received = recv(connected_client_sockets[thread_number].sock, request_buff, sizeof(request_buff), 0);
		if (bytes_received == SOCKET_ERROR)
		{
			cout << "ERROR in receiving request from client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
			cout << "ERROR number: " << WSAGetLastError() << endl;
			break;
		}
		else if (bytes_received == 0)
		{
			client_disconnection_message(connected_client_sockets[thread_number].IP_address, connected_client_sockets[thread_number].port_num);
			break;
		}

		cout << connected_client_sockets[thread_number].IP_address << ", thread_number " << thread_number << " requested " << request_buff << "." << endl;
		
		char request_acceptance_result_buff[2];
		request_acceptance_result_buff[0] = 'y';
		request_acceptance_result_buff[1] = '\0';

		//send request acceptance result to client
		int bytes_sent = send(connected_client_sockets[thread_number].sock, request_acceptance_result_buff, sizeof(request_acceptance_result_buff), 0);
		if (bytes_sent == SOCKET_ERROR)
		{
			cout << "ERROR in sending request acceptance result to client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
			cout << "ERROR number: " << WSAGetLastError() << endl;
			break;
		}
		else if (bytes_sent == 0)
		{
			client_disconnection_message(connected_client_sockets[thread_number].IP_address, connected_client_sockets[thread_number].port_num);
			break;
		}

		char current_request = request_buff[0];
		cout << current_request << " request from " << connected_client_sockets[thread_number].IP_address << " accepted. Processing." << endl;
		process_requests(current_request, thread_number, connected_client_sockets, master_mutex, memory, cache);
		//process results will be sent in send_process_result function within process_requests function above
	}
}

//code to wait for and connect to clients real time
void wait_for_clients(	vector<socket_info>& connected_client_sockets,
						vector<thread>& socket_threads,
						socket_info& listening_socket,
						mutex& master_mutex,
						double memory[NUMMEMORY],
						account_cache_set cache[CACHENUMOFSETS])
{
	int thread_number = 0;
	while (true)
	{
		//Wait for connection
		socket_info client_socket;
		int client_size = sizeof(client_socket.sock_addr);
		client_socket.sock = accept(listening_socket.sock, (sockaddr*)& client_socket.sock_addr, &client_size);

		if (client_socket.sock == INVALID_SOCKET)
		{
			exit_with_err_msg("client_socket creation failed.");
		}

		set_client_connection_info(client_socket);
		cout << "Client IP address " << client_socket.IP_address << " has connected to port " << client_socket.port_num << "." << endl;

		master_mutex.lock();
		connected_client_sockets.push_back(client_socket);

		//new thread for each client connection
		socket_threads.push_back(thread(accept_requests, thread_number++, ref(connected_client_sockets), ref(master_mutex), memory, cache));

		master_mutex.unlock();
	}
}

void send_process_result(	const char process_result_buff[2],
							const int thread_number,
							vector<socket_info>& connected_client_sockets,
							mutex& master_mutex)
{
	//send process result to client
	char temp_process_result_buff[2];
	temp_process_result_buff[0] = process_result_buff[0];
	temp_process_result_buff[1] = process_result_buff[1];

	//if process_result_buff (function argument) is directly passed onto send(), it's 4 bytes instead of 2
	int bytes_sent = send(connected_client_sockets[thread_number].sock, temp_process_result_buff, sizeof(temp_process_result_buff), 0);
	if (bytes_sent == SOCKET_ERROR)
	{
		cout << "ERROR in sending request result to client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
		cout << "ERROR number: " << WSAGetLastError() << endl;
		return;
	}
	else if (bytes_sent == 0)
	{
		client_disconnection_message(connected_client_sockets[thread_number].IP_address, connected_client_sockets[thread_number].port_num);
		return;
	}

	cout << "Sent process result " << process_result_buff << " to client " << connected_client_sockets[thread_number].IP_address << "." << endl;
}

void receive_message(	const int thread_number,
						vector<socket_info>& connected_client_sockets,
						mutex& master_mutex)
{
	char msg_buff[4096];

	//receive message sent from the client
	int bytes_received = recv(connected_client_sockets[thread_number].sock, msg_buff, sizeof(msg_buff), 0);
	if (bytes_received == SOCKET_ERROR)
	{
		cout << "ERROR in receiving request from client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
		cout << "ERROR number: " << WSAGetLastError() << endl;
		return;
	}
	else if (bytes_received == 0)
	{
		client_disconnection_message(connected_client_sockets[thread_number].IP_address, connected_client_sockets[thread_number].port_num);
		return;
	}

	cout << connected_client_sockets[thread_number].IP_address << ", thread_number " << thread_number << " messaged: \"" << msg_buff << "\"" << endl;
}

void process_request_m(	const int thread_number,
						vector<socket_info>& connected_client_sockets,
						mutex& master_mutex)
{
	char process_result_buff[2];
	process_result_buff[1] = '\0';
	receive_message(thread_number, connected_client_sockets, master_mutex);
	process_result_buff[0] = 's'; //message received successfully
	send_process_result(process_result_buff, thread_number, connected_client_sockets, master_mutex);
}

void process_request_r(	const int thread_number,
						vector<socket_info>& connected_client_sockets,
						mutex& master_mutex,
						double memory[NUMMEMORY],
						account_cache_set cache[CACHENUMOFSETS])
{
	char process_result_buff[2];
	process_result_buff[1] = '\0';
	int account_number;
	int bytes_received = recv(connected_client_sockets[thread_number].sock, (char*)& account_number, sizeof(account_number), 0);
	if (bytes_received == SOCKET_ERROR)
	{
		cout << "ERROR in receiving account_number from client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
		cout << "ERROR number: " << WSAGetLastError() << endl;
		return;
	}
	else if (bytes_received == 0)
	{
		cout << "Client " << connected_client_sockets[thread_number].IP_address << " has disconnected." << endl;
		return;
	}
	cout << "Client " << connected_client_sockets[thread_number].IP_address << " requested the balance of account number " << account_number << "." << endl;

	double requested_balance = read_account(account_number, master_mutex, memory, cache);
	process_result_buff[0] = requested_balance == -1 ? 'f' : 's'; //failed if -1, success if not
	send_process_result(process_result_buff, thread_number, connected_client_sockets, master_mutex);
	
	if (requested_balance != -1)
	{
		int bytes_sent = send(connected_client_sockets[thread_number].sock, (char*)& requested_balance, sizeof(requested_balance), 0);
		if (bytes_sent == SOCKET_ERROR)
		{
			cout << "ERROR in sending requested balance to client " << connected_client_sockets[thread_number].IP_address << ". Exiting." << endl;
			cout << "ERROR number: " << WSAGetLastError() << endl;
			return;
		}
		else if (bytes_sent == 0)
		{
			client_disconnection_message(connected_client_sockets[thread_number].IP_address, connected_client_sockets[thread_number].port_num);
			return;
		}
		
		cout << "Sent requested_balance of $" << requested_balance << " to client " << connected_client_sockets[thread_number].IP_address << "." << endl;
	}
}

double read_account(const int account_number,
					mutex& master_mutex,
					double memory[NUMMEMORY],
					account_cache_set cache[CACHENUMOFSETS])
{
	load_onto_cache(account_number, memory, cache);
	double requested_balance = read_from_cache(account_number, cache);
	return requested_balance;
}

void process_requests(	const char current_request,
						const int thread_number,
						vector<socket_info>& connected_client_sockets,
						mutex& master_mutex,
						double memory[NUMMEMORY],
						account_cache_set cache[CACHENUMOFSETS])
{
	master_mutex.lock();
	if (current_request == 'm')//message
	{
		process_request_m(thread_number, connected_client_sockets, master_mutex);
	}
	else if (current_request == 'r') //read
	{
		process_request_r(thread_number, connected_client_sockets, master_mutex, memory, cache);
	}
	else if (current_request == 'u') //update
	{
		//code to update account balance
	}
	else { exit_with_err_msg("Client " + string(connected_client_sockets[thread_number].IP_address) + " has sent an invalid request. Request sent: " + current_request); }
	master_mutex.unlock();
}