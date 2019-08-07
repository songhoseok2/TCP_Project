#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <WS2tcpip.h>

#define NUMMEMORY 10000
#define CACHENUMOFLINE 30
#define SETBLOCKSIZE 20
#define CACHENUMOFSETS 10

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
	int tag_id = -1;
	double cache_lines[CACHENUMOFLINE];
};

struct account_cache_set
{
	bool dirty = false;
	vector<int> usage_deque; //for the purpose of LRU. most recent tagblock is at the back
	cache_tag blocks[SETBLOCKSIZE];
};

void initialize_winsock();

char get_request();

string get_message();

int get_port_number();

#endif