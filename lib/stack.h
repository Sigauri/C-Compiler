
#ifndef STACK_H_INCLUDED
#include "stddef.h"
#define STACK_DEFAULT_SIZE 64

struct stack
{
	// stack size
	size_t st_size;

	// Array of items
	void **st_items;
	
	// Pointer to the top of stack
	void **st_top;
};

void init_stack(struct stack **st_addr);
void remove_stack(struct stack *st);
void push(struct stack *st, void *item);
void pop(struct stack *st);
void *peek(struct stack *st);
#define STACK_H_INCLUDED
#endif