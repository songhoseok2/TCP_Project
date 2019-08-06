#include "cache_system.h"
#include <WS2tcpip.h>

using namespace std;

int get_set_index(const int account_number) { return account_number % CACHESET; }

int get_tag_index(const int account_number) { return floor(account_number / (CACHESET * CACHETAG)); }

int get_block_offset(const int account_number) { return account_number % CACHELINE; }