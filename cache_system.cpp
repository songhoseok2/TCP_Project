#include "cache_system.h"
#include <WS2tcpip.h>

using namespace std;

int get_tag_block_offset(const int account_number) { return account_number % TAGBLOCKSIZE; }

int get_tag_id(const int account_number) { return (account_number - get_tag_block_offset(account_number)) / (CACHENUMOFSETS * SETBLOCKSIZE * TAGBLOCKSIZE); }

//(account_number - get_tag_block_offset(account_number) - get_tag_id(account_number) * CACHENUMOFSETS * SETBLOCKSIZE * TAGBLOCKSIZE) / (SETBLOCKSIZE * TAGBLOCKSIZE); }
int get_set_id(const int account_number) { return (account_number / (SETBLOCKSIZE * TAGBLOCKSIZE)) % CACHENUMOFSETS; }

int get_account_number(const int set_id, const int tag_id, const int tag_block_offset) { return set_id * CACHENUMOFSETS + tag_id * SETBLOCKSIZE + tag_block_offset; }

int get_cache_tag_index(const int tag_id, cache_tag blocks[SETBLOCKSIZE])
{
	auto it = find_if(blocks, blocks + SETBLOCKSIZE, [tag_id](cache_tag current_tag) { return current_tag.tag_id == tag_id; });
	return it != (blocks + SETBLOCKSIZE) ? (it - blocks) : -1;
}

void update_LRU(const int most_recent_tag_index, account_cache_set& current_set)
{
	//check if the update is necessary (see if the recent tag is already at the back)
	if (!current_set.usage_deque.empty() && current_set.usage_deque.back() == most_recent_tag_index) { return; }

	if (current_set.usage_deque.empty()) { current_set.usage_deque.push_back(most_recent_tag_index); }

	//if not all tag blocks are being used, LRU(next block to use) should be an empty one so that eviction isn't necessary
	if (current_set.usage_deque.size() < SETBLOCKSIZE)
	{
		//find an empty tag_block and push_front
		cache_tag* it = find_if(current_set.tag_blocks, current_set.tag_blocks + SETBLOCKSIZE, [](cache_tag current_tag_block) { return current_tag_block.tag_id == -1; });
		if (it != current_set.tag_blocks + SETBLOCKSIZE) { current_set.usage_deque.push_front(it - current_set.tag_blocks); }
		else { assert(false); } //this is wrong. in this code block there must be an empty tag block.
	}

	auto it = find(current_set.usage_deque.begin(), current_set.usage_deque.end(), most_recent_tag_index);
	if (it != current_set.usage_deque.end()) { current_set.usage_deque.erase(it); }
	current_set.usage_deque.push_back(most_recent_tag_index);
}


void write_to_memory(const int set_id, const int tag_index, double memory[NUMMEMORY], account_cache_set cache[CACHENUMOFSETS])
{
	for (int i = 0; i < TAGBLOCKSIZE; ++i)
	{
		memory[get_account_number(set_id, cache[set_id].tag_blocks[tag_index].tag_id, i)] = cache[set_id].tag_blocks[tag_index].cache_lines[i];
	}
	cache[set_id].tag_blocks[tag_index].dirty = false;
}

int load_tag_block(const int account_number, double memory[NUMMEMORY], account_cache_set cache[CACHENUMOFSETS])
{
	int set_id = get_set_id(account_number);
	int LRU_index; //LRU is the index of the tag that is the LRU. not the tag_id

	//if this set is newly loaded and is empty
	LRU_index = cache[set_id].usage_deque.empty() ? 0 : cache[set_id].usage_deque[0];
	if (cache[set_id].tag_blocks[LRU_index].dirty)
	{
		write_to_memory(set_id, LRU_index, memory, cache);
	}
	int current_account_tag_block_offset = get_tag_block_offset(account_number);
	int beginning_tag_account_number = account_number - current_account_tag_block_offset;
	for (int i = 0; i < TAGBLOCKSIZE; ++i)
	{
		cache[set_id].tag_blocks[LRU_index].cache_lines[i] = memory[beginning_tag_account_number + i];
	}

	cache[set_id].tag_blocks[LRU_index].tag_id = get_tag_id(account_number);
	update_LRU(LRU_index, cache[set_id]);
	return LRU_index;
}

void load_onto_cache(const int account_number, double memory[NUMMEMORY], account_cache_set cache[CACHENUMOFSETS])
{
	int set_id = get_set_id(account_number);
	bool is_tag_block_loaded = true;

	//set miss
	if (cache[set_id].usage_deque.empty()) { is_tag_block_loaded = false; }

	int cache_tag_index = get_cache_tag_index(get_tag_id(account_number), cache[set_id].tag_blocks);
	//tag miss
	if (cache_tag_index == -1) { is_tag_block_loaded = false; }
	if (!is_tag_block_loaded) { cache_tag_index = load_tag_block(account_number, memory, cache); }

	update_LRU(cache_tag_index, cache[set_id]);
}

double read_from_cache(const int account_number, account_cache_set cache[CACHENUMOFSETS])
{
	int set_id = get_set_id(account_number);
	int memory_tag_id = get_tag_id(account_number);
	int cache_tag_index = get_cache_tag_index(memory_tag_id, cache[set_id].tag_blocks);
	double result_balance = cache[set_id].tag_blocks[cache_tag_index].cache_lines[get_tag_block_offset(account_number)];
	update_LRU(cache_tag_index, cache[set_id]);
	return result_balance;
}

char write_account(const int account_number,
	const double new_balance,
	double memory[NUMMEMORY],
	account_cache_set cache[CACHENUMOFSETS])
{
	load_onto_cache(account_number, memory, cache);
	int set_id = get_set_id(account_number);
	int cache_tag_index = get_cache_tag_index(account_number, cache[set_id].tag_blocks);
	//if the account balance is already the inteded new_balance, there is no need for an update.
	if (cache[set_id].tag_blocks[cache_tag_index].cache_lines[get_tag_block_offset(account_number)] == new_balance) { return 'n'; }
	
	cache[set_id].tag_blocks[cache_tag_index].cache_lines[get_tag_block_offset(account_number)] = new_balance;
	cache[set_id].tag_blocks->dirty = true;
	update_LRU(cache_tag_index, cache[set_id]);
	return 's';
}