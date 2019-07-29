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

int get_port_number()
{
	string listen_port_num_str;
	cout << "Enter the port number: ";
	while (getline(cin, listen_port_num_str))
	{
		bool invalid_input = any_of(listen_port_num_str.begin(), listen_port_num_str.end(), [](const char c) { return !isdigit(c); });
		if (invalid_input) { cout << "ERROR: Please enter numbers only: "; }
		else { break; }
	}

	return stoi(listen_port_num_str);
}