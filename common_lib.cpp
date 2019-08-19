#include "common_lib.h"
#include <WS2tcpip.h>

using namespace std;

void exit_with_err_msg(const string msg)
{
	cout << msg << endl;
	cout << "Cleaning WSA...";
	WSACleanup();
	cout << "Done. Exiting." << endl;
	exit(1);
}

bool is_disconnected(const int bytes)
{
	return (bytes == SOCKET_ERROR && (int)WSAGetLastError() == 10054) || bytes == 0;
}

bool is_char::operator()(char c) { return !isdigit(c); }

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

char get_request()
{
	string request_str;
	cout << "Enter request: ";
	while (getline(cin, request_str))
	{
		if (request_str.size() != 1) { cout << "ERROR: Please enter one character only: "; }
		else if (request_str == "q" ||
			request_str == "m" ||
			request_str == "r" ||
			request_str == "u") {
			break;
		}
		else { cout << request_str << " isn't a valid request. Please re-enter: "; }
	}

	return request_str[0];

}

string get_message()
{
	string user_msg;
	cout << "Enter message to send. Enter ABORT to abort." << endl;
	cout << "Message to server: ";
	while (getline(cin, user_msg))
	{
		if (user_msg.empty())
		{
			cout << "Message is empty. Please re-enter. Enter ABORT to abort." << endl;
			cout << "Message to send: ";
		}
		else { break; }
	}

	return user_msg;
}

int get_port_number()
{
	string listen_port_num_str;
	cout << "Enter the port number: ";
	while (getline(cin, listen_port_num_str))
	{
		bool invalid_input = any_of(listen_port_num_str.begin(), listen_port_num_str.end(), is_char());
		if (invalid_input) { cout << "ERROR: Please enter numbers only: "; }
		else if (listen_port_num_str.empty()) { cout << "Input is empty. Please re-enter: "; }
		else { break; }
	}

	return stoi(listen_port_num_str);
}