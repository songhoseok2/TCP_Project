#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#include <iostream>
#include <string>
#include <algorithm>
#include <WS2tcpip.h>

using namespace std;

void exit_with_err_msg(const string msg);

struct socket_info
{
	SOCKET sock;
	sockaddr_in sock_addr;
	char IP_address[INET_ADDRSTRLEN];
	int port_num;
};

void initialize_winsock();

char get_request();

string get_message();

int get_port_number();

#endif