#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#include <iostream>
#include <string>
#include <WS2tcpip.h>

using namespace std;

void exit_with_err_msg(const string msg);

struct socket_info
{
	SOCKET sock;
	sockaddr_in sock_addr;
};

void initialize_winsock();

#endif