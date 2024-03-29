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
Cache system design:
	-	Set associative cache
	-	Write-back policy
	-	LRU (Least Recently Used) within each set
	-	Dirty-bit
*/

extern mutex cache_set_mutex[CACHENUMOFSETS];
extern mutex memory_mutex[NUMMEMORY / TAGBLOCKSIZE];
extern mutex thread_vec_mutex;
extern mutex msg_mutex;

int get_tag_block_offset(const int account_number);

int get_tag_id(const int account_number);

int get_set_id(const int account_number);

int get_account_number(const int set_index, const int tag_id, const int tag_block_offset);

int get_cache_tag_index(const int tag_id, cache_tag blocks[CACHESETBLOCKSIZE]);

void update_LRU(const int most_recent_tag_index, account_cache_set& current_set);

void write_to_memory(const int set_index,
	const int tag_index,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

//return ths newly loaded tag's cacheINDEX (not the id)
int load_tag_block(const int account_number,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

void load_onto_cache(const int account_number,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

//this function assumes that correct tag block and the data is already loaded in cache
double read_from_cache(const int account_number, account_cache_set cache[CACHENUMOFSETS]);

char write_account(const int account_number,
	const double new_balance,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS]);

#endif