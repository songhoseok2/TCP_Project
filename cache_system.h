#ifndef CACHE_SYSTEM_H
#define CACHE_SYSTEM_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <mutex>
#include <thread>
#include "common_lib.h"
#include <WS2tcpip.h>

using namespace std;
/*
NOTE:

Cache system design:
	-	Set associative cache
	-	Write-back policy
	-	LRU (Least Recently Used)
	-	Dirty-bit


*/

int get_set_index(const int account_number);

int get_tag_index(const int account_number);

int get_block_offset(const int account_number);

#endif