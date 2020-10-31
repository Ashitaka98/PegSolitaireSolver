#ifndef __AI__
#define __AI__

#include <stdint.h>
#include <unistd.h>
#include "utils.h"
#include "hashtable.h"

void initialize_ai();

void find_solution( state_t* init_state );
void free_memory(HashTable* ht);

#endif
