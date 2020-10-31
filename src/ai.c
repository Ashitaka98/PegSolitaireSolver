#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#include "ai.h"
#include "utils.h"
#include "hashtable.h"
#include "stack.h"

extern state_t solution[SIZE*SIZE];
extern move_t solution_moves[SIZE*SIZE];
extern int solution_size;
extern int generated_nodes;
extern int explored_nodes;
extern int budget;

void initialize_ai()
{

}

void free_memory(HashTable* ht)
{
	HTNode* node;
	HTNode* next;
	size_t chain;

	// release all node_t stored in ht
	for (chain = 0; chain < ht->capacity; ++chain) {
		node = ht->nodes[chain];
		while (node) {
			next = node->next;
			free(*(node_t**)node->value);	// release a node_t by its pointer stored as value
			node = next;
		}
	}

	ht_destroy(ht); //call destructor of ht
}

void copy_state(state_t* dst, state_t* src){
	
	//Copy field
	memcpy( dst->field, src->field, SIZE*SIZE*sizeof(int8_t) );

	dst->cursor = src->cursor;
	dst->selected = src->selected;
}

/*
  Saves the path up to the node as the best solution found so far
*/
void save_solution( node_t* solution_node ){
	node_t* n = solution_node;
	while( n->parent != NULL ){
		copy_state( &(solution[n->depth]), &(n->state) );
		solution_moves[n->depth-1] = n->move;

		n = n->parent;
	}
	solution_size = solution_node->depth;
}


node_t* create_init_node( state_t* init_state ){
	node_t * new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = NULL;	
	new_n->depth = 0;
	copy_state(&(new_n->state), init_state);
	return new_n;
}

/**
 * Apply an action to node n and return a new node resulting from executing the action
*/
node_t* applyAction(node_t* n, position_s* selected_peg, move_t action ){

    node_t* new_node = NULL;

	//FILL IN MISSING CODE
	new_node = (node_t*)malloc(sizeof(node_t));
	if(new_node==NULL)
	{
		printf("failed to malloc a node\n");
		exit(0);
	}
	copy_state(&new_node->state,&n->state);
	new_node->parent = n;
	new_node->depth = n->depth+1;
	new_node->move = action;
	new_node->state.cursor = *selected_peg;
	

    execute_move_t( &(new_node->state), action );
	
	return new_node;

}

/**
 * Find a solution path as per algorithm description in the handout
 */

void find_solution( state_t* init_state  ){

	// we will build a hashtable whose key is the state(only board, without cursor)
	// and its value is the node corresponding
	HashTable table;

	// Choose initial capacity of PRIME NUMBER 
	// Specify the size of the keys and values you want to store once 
	ht_setup( &table, sizeof(int8_t) * SIZE * SIZE, sizeof(node_t*), 16769023);

	// Initialize Stack
	initialize_stack();

	//Add the initial node
	node_t* n = create_init_node( init_state );
	
	//FILL IN THE GRAPH ALGORITHM
	stack_push(n);
	ht_insert(&table,&(n->state.field[0][0]),&n);
	int remaining_pegs = num_pegs(&n->state);

	while(!is_stack_empty())
	{
		n = stack_top();
		stack_pop();
		explored_nodes++;
		int n_peg = num_pegs(&n->state);
		if(n_peg<remaining_pegs)
		{
			save_solution(n);
			remaining_pegs = n_peg;
		} 
		for(int i=0;i<SIZE;i++)
		{
			for(int j=0;j<SIZE;j++)
			{
				position_s pos;
				pos.x = j;
				pos.y = i;
				for(move_t m=0;m<4;m++)
				{

					if(can_apply(&n->state,&pos,m))
					{
						node_t* new_node = applyAction(n,&pos,m);
						generated_nodes++;

						if(won(&new_node->state))
						{
							save_solution(new_node);
							remaining_pegs = num_pegs(&new_node->state);
							return;
						}
						if(!ht_contains(&table,&(new_node->state.field[0][0]))){
							stack_push(new_node);
							ht_insert(&table,&(new_node->state.field[0][0]),&new_node);
						}
						else{	// the same board appeared before, and we do not care the path(status of stack)
							free(new_node);	// so the node can be deleted
						}
					}
				}
			}
		}
		if(explored_nodes>=budget)
			break;
	}
	free_memory(&table);	// release existing node whose pointer stored in hashtable as value
	return;
}