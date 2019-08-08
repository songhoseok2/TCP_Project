#include "cache_system.h"
#include <WS2tcpip.h>

using namespace std;

int get_set_index(const int account_number) { return account_number % CACHENUMOFSETS; }

int get_tag_index(const int account_number) { return floor(account_number / (CACHENUMOFSETS * SETBLOCKSIZE)); }

int get_tag_block_offset(const int account_number) { return account_number % TAGBLOCKSIZE; }

int get_account_number(const int set_index, const int tag_index, const int tag_block_offset)
{
	return set_index * CACHENUMOFSETS + tag_index * SETBLOCKSIZE + tag_block_offset;
}

int search_set_blocks(const int tag_to_search, cache_tag blocks[SETBLOCKSIZE])
{
	auto it = find_if(blocks, blocks + SETBLOCKSIZE, [tag_to_search](cache_tag current_tag) { return (current_tag.tag_id == tag_to_search); });
	return ((it != (blocks + SETBLOCKSIZE)) ? (it - blocks) : -1);	
}

void update_LRU(const int most_recent_tag, account_cache_set & current_set)
{
	//check if the update is necessary (see if the recent tag is already at the back)
	if (current_set.usage_deque.back() == most_recent_tag) { return; }

	//if not all tag blocks are being used, LRU(next block to use) should be an empty one so that eviction isn't necessary
	else if (current_set.usage_deque.size() < CACHENUMOFSETS)
	{
		//find an empty tag_block and push_front
		auto it = find_if(current_set.tag_blocks, current_set.tag_blocks + SETBLOCKSIZE, [](cache_tag current_tag_block) { return current_tag_block.tag_id == -1; });
		if (it != current_set.tag_blocks + SETBLOCKSIZE) { current_set.usage_deque.push_front(it - current_set.tag_blocks); }
		else { assert(false); } //this is wrong. in this code block there must be an empty tag block.
	}

	else //update normally
	{
		current_set.usage_deque.erase(find(current_set.usage_deque.begin(), current_set.usage_deque.end(), most_recent_tag));
		current_set.usage_deque.push_back(most_recent_tag);
	}
}


void write_to_memory(const int set_index, const int tag_index, double memory[NUMMEMORY], account_cache_set cache[CACHENUMOFSETS])
{
	for (int i = 0; i < TAGBLOCKSIZE; ++i)
	{
		memory[get_account_number(set_index, tag_index, i)] = cache[set_index].tag_blocks[tag_index].cache_lines[i];
	}
	cache[set_index].tag_blocks[tag_index].dirty = false;
}

void load_tag_block(const int account_number, double memory[NUMMEMORY], account_cache_set cache[CACHENUMOFSETS])
{
	int set_index = get_set_index(account_number);
	int LRU;

	if (cache[set_index].usage_deque.empty()) //if this set is newly loaded and is empty
	{

	}
	else
	{
		LRU = cache[set_index].usage_deque[0];
		if (cache[set_index].tag_blocks[LRU].dirty)
		{

		}

		int current_tag_block_offset = get_tag_block_offset(account_number);
		int beginning_tag_account_number = account_number - current_tag_block_offset;
		for (int i = 0; i < TAGBLOCKSIZE; ++i)
		{
			cache[set_index].tag_blocks[LRU].cache_lines[i] = memory[beginning_tag_account_number + i];
		}
	}
	cache[set_index].tag_blocks[LRU].tag_id = LRU;
	update_LRU(LRU, cache[set_index]);
}

void load_onto_cache(const int account_number, double memory[NUMMEMORY], account_cache_set cache[CACHENUMOFSETS])
{
	int set_index = get_set_index(account_number);
	if (cache[set_index].usage_deque.empty()) //set miss
	{
		load_tag_block(account_number, memory, cache);
		return;
	}

	int memory_tag_index = get_tag_index(account_number);
	int cache_tag_index = search_set_blocks(memory_tag_index, cache[set_index].tag_blocks);
	if (cache_tag_index = -1) //tag miss
	{

	}

	update_LRU();
}



double read_from_cache(const int account_number, account_cache_set cache[CACHENUMOFSETS])
{
	int set_index = get_set_index(account_number);
	int memory_tag_index = get_tag_index(account_number);
	int cache_tag_index = search_set_blocks(memory_tag_index, cache[set_index].tag_blocks);
	double result_balance = cache[set_index].tag_blocks[cache_tag_index].cache_lines[get_tag_block_offset(account_number)];
	update_LRU(cache_tag_index, cache[set_index]);
	return result_balance;
}