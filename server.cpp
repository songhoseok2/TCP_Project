#include <iostream>
#include <string>
#include <algorithm>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void exit_with_err_msg(const string msg)
{
	cout << msg << endl;
	cout << "Cleaning WSA...";
	WSACleanup();
	cout << "Done. Exiting." << endl;
	exit(1);
}

struct socket_info
{
	SOCKET sock;
	sockaddr_in sock_addr;
};

void initialize_winsock()
{
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int ws_result = WSAStartup(ver, &wsData);
	if (ws_result != 0)
	{
		exit_with_err_msg("Winsock initialization failed. Error " + to_string(ws_result) + ".");
	}

	cout << "Winsock initialization successful." << endl;
}

socket_info create_listening_socket()
{
	socket_info listening_socket;

	listening_socket.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_socket.sock == INVALID_SOCKET)
	{
		exit_with_err_msg("listen_socket creation failed. Error #" + to_string(WSAGetLastError()) + ".");
	}

	string listen_port_num_str;
	cout << "Enter the port number you wish to assign for the listening socket: ";
	while (getline(cin, listen_port_num_str))
	{
		bool invalid_input = any_of(listen_port_num_str.begin(), listen_port_num_str.end(), [](const char c) { return !isdigit(c); });
		if (invalid_input) { cout << "ERROR: Please enter numbers only: "; }
		else { break; }
	}
	int listen_port_num = stoi(listen_port_num_str);

	//setup the host_addr structure for use in bind call
	listening_socket.sock_addr.sin_family = AF_INET;				//server byte order
	listening_socket.sock_addr.sin_addr.S_un.S_addr = INADDR_ANY;	//automatically be filled with current host's IP address
	listening_socket.sock_addr.sin_port = htons(listen_port_num);				//convert short integer value for port must be converted into network byte order
	
	return listening_socket;
}

socket_info connect_to_a_client(socket_info listening_socket)
{
	//Wait for connection
	socket_info client_socket;
	int client_size = sizeof(client_socket.sock_addr);
	client_socket.sock = accept(listening_socket.sock, (sockaddr*)& client_socket.sock_addr, &client_size);

	if (client_socket.sock == INVALID_SOCKET)
	{
		exit_with_err_msg("client_socket creation failed.");
	}

	char host[NI_MAXHOST];			//Client's remote name
	char service[NI_MAXSERV];		//Service (i.e. port) the client is connected on

	ZeroMemory(host, NI_MAXHOST);	//memset(host, 0, NI_MAXHOST)
	ZeroMemory(service, NI_MAXSERV);


	cout << host << "connected on port ";
	if (getnameinfo((sockaddr*)& client_socket.sock, client_size, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << service << endl;
		cout << "getnameinfo successful." << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client_socket.sock_addr.sin_addr, host, NI_MAXHOST);
		cout << ntohs(client_socket.sock_addr.sin_port) << endl;
		cout << "getnameinfo failed." << endl;
	}

	return client_socket;
}

void temporary_action(socket_info &client_socket)
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

int main()
{
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

	socket_info client_socket = connect_to_a_client(listening_socket);
	temporary_action(client_socket);

	closesocket(client_socket.sock);
	WSACleanup();

	return 0;
}