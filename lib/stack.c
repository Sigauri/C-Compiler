#include "stack.h"
#include "stdlib.h"
#include "stdio.h"
#include "stddef.h"


/* 
	TODOs:
		test it
		make the stack resizeable
*/


void init_stack(struct stack **st_addr)
{
	*st_addr = malloc(sizeof(struct stack)); 
	struct stack *st = *st_addr;
	st->st_size = STACK_DEFAULT_SIZE;

	// allocate array of pointers to stack items
	st->st_items = calloc(st->st_size, sizeof(size_t));
	st->st_top = st->st_items;
}

// free all the space allocated for stack
void remove_stack(struct stack *st)
{	
	free(st->st_items);
	st->st_items = NULL;
}



void push(struct stack *st, void *item)
{
	*st->st_top = item;
	st->st_top++;
}

void pop(struct stack *st)
{
	if(st->st_top != st->st_items)
		st->st_top--;
}

void *peek(struct stack *st)
{
	if(st->st_top == st->st_items)
		return *st->st_items;
	return *(st->st_top-1);
}

