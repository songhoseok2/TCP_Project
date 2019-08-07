#include "cache_system.h"
#include <WS2tcpip.h>

using namespace std;

int get_set_index(const int account_number) { return account_number % CACHENUMOFSETS; }

int get_tag_index(const int account_number) { return floor(account_number / (CACHENUMOFSETS * SETBLOCKSIZE)); }

int get_tag_block_offset(const int account_number) { return account_number % CACHENUMOFLINE; }

int search_set_blocks(const int tag_to_search, cache_tag blocks[SETBLOCKSIZE])
{
	auto it = find_if(blocks, blocks + SETBLOCKSIZE, [tag_to_search](cache_tag current_tag) { return (current_tag.tag_id == tag_to_search); });
	return ((it != (blocks + SETBLOCKSIZE)) ? (it - blocks) : -1);	
}

void update_LRU()
{
	//code to update usage value and return the new LRU (if changed)
}

void load_tag_block(const int account_number, double memory[NUMMEMORY], account_cache_set cache[CACHENUMOFSETS])
{

}

void load_onto_cache(const int account_number, double memory[NUMMEMORY], account_cache_set cache[CACHENUMOFSETS])
{
	int set_index = get_set_index(account_number);
	if (cache[set_index].usage_deque.empty()) //set miss
	{
		load_tag_block(account_number, memory, cache);
		update_LRU();
		return;
	}

	int memory_tag_index = get_tag_index(account_number);
	int cache_tag_index = search_set_blocks(memory_tag_index, cache[set_index].blocks);
	if (cache_tag_index = -1) //tag miss
	{

	}

	update_LRU();
}



double read_from_cache(const int account_number, account_cache_set cache[CACHENUMOFSETS])
{
	int set_index = get_set_index(account_number);
	int memory_tag_index = get_tag_index(account_number);
	int cache_tag_index = search_set_blocks(memory_tag_index, cache[set_index].blocks);
	double result_balance = cache[set_index].blocks[cache_tag_index].cache_lines[get_tag_block_offset(account_number)];
	update_LRU();
	return result_balance;
}