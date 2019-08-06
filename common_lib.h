#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#include <iostream>
#include <string>
#include <algorithm>
#include <WS2tcpip.h>

#define NUMMEMORY 10000
#define CACHELINE 50
#define CACHETAG 5
#define CACHESET 250

using namespace std;

void exit_with_err_msg(const string msg);

struct is_char { bool operator()(char c); };

struct socket_info
{
	SOCKET sock;
	sockaddr_in sock_addr;
	char IP_address[INET_ADDRSTRLEN];
	int port_num;
};

struct cache_tag
{
	double cache_lines[CACHELINE];
};

struct account_cache_set
{
	int dirty;
	int LRU;
	cache_tag tags[CACHETAG];
};

void initialize_winsock();

char get_request();

string get_message();

int get_port_number();

#endif