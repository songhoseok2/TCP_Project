#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
	//Initialize Winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int ws_result = WSAStartup(ver, &wsData);
	if (ws_result != 0)
	{
		cout << "Winsock initialization failed. Error " << ws_result << ". Exiting." << endl;
		return 0;
	}

	//Create a socket
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		cout << "listen_socket creation failed. Error #" << WSAGetLastError() << ". Exiting." << endl;
		return 0;
	}

	sockaddr_in serv_addr;
	//setup the host_addr structure for use in bind call

	serv_addr.sin_family = AF_INET;					//server byte order
	serv_addr.sin_addr.S_un.S_addr = INADDR_ANY;	//automatically be filled with current host's IP address
	serv_addr.sin_port = htons(54000);				//convert short integer value for port must be converted into network byte order


	//Bind ip address and port to a socket
	if (bind(listen_sock, (sockaddr*)& serv_addr, sizeof(serv_addr)) < 0)
	{
		cout << "Binding failed. Exiting." << endl;
		return 0;
	}

	//listen() tells the socket to listen to the incoming connections.
	//The listen() function places all incoming connection into a backlog queue
	//until accept() call accepts the connection.
	//Setting the maximum size for the backlog queue to a defined number.
	listen(listen_sock, SOMAXCONN);


	//Wait for connection
	sockaddr_in client_addr;
	int client_size = sizeof(client_addr);
	SOCKET client_sock = accept(listen_sock, (sockaddr*)& client_addr, &client_size);

	if (client_sock == INVALID_SOCKET)
	{
		cout << "client_socket creation failed. Exiting" << endl;
		return 0;
	}

	char host[NI_MAXHOST];			//Client's remote name
	char service[NI_MAXSERV];		//Service (i.e. port) the client is connected on

	ZeroMemory(host, NI_MAXHOST);	//memset(host, 0, NI_MAXHOST)
	ZeroMemory(service, NI_MAXSERV);


	cout << host << "connected on port ";
	if (getnameinfo((sockaddr*)& client_sock, client_size, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout <<  service << endl;
		cout << "getnameinfo successful." << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
		cout << ntohs(client_addr.sin_port) << endl;
		cout << "getnameinfo failed." << endl;
	}

	//Close listening socket
	closesocket(listen_sock);

	//While loop to accept and echo message back to client
	char buff[4096];
	for(ZeroMemory(buff, 4096); true; ZeroMemory(buff, 4096))
	{
		//wait for client to send data
		int bytes_received = recv(client_sock, buff, 4096, 0);
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
		int bytes_sent = send(client_sock, buff, bytes_received + 1, 0); //+1 cuz of the terminating \0
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

	//Close the socket
	closesocket(client_sock);

	//Cleanup Winsock
	WSACleanup();

	return 0;
}