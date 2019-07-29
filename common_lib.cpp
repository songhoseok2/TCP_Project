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