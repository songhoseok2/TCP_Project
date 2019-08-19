#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#include <iostream>
#include <string>
#include <algorithm>
#include <deque>
#include <cassert>
#include <WS2tcpip.h>

#define NUMMEMORY 10000

//small numbers for now for debugging purpose
#define TAGBLOCKSIZE 4
#define SETBLOCKSIZE 2
#define CACHENUMOFSETS 5

using namespace std;

void exit_with_err_msg(const string msg);

bool is_disconnected(const int bytes);

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
	bool dirty = false;
	int tag_id = -1;
	double cache_lines[TAGBLOCKSIZE];
};

struct account_cache_set
{	
	//for the purpose of LRU. most recent tagblock is at the back. LRU is at the front.
	deque<int> usage_deque;

	cache_tag tag_blocks[SETBLOCKSIZE];
};

void initialize_winsock();

char get_request();

string get_message();

int get_port_number();

#endif