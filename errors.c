#include "errors.h"
#include "lexer/stack.h"
#include "stdlib.h"
#include "stdio.h"

/* 
	Only basic stuff for handling errors is implemented here,
	so we can use it in any part of compiler (parser, lexer, code generator etc.)
	Other features that are only needed in a particular module will
	be implemented there (like panic-mode for the parser).
*/

void init_error_state(struct error_state **estate)
{
	*estate = malloc(sizeof(struct error_state));
	(*estate)->error = 0;
	
	init_stack(&(*estate)->error_stack);
}

void add_error(struct error_state *estate, char *msg)
{
	push(estate->error_stack, (void*)msg);
}

void display_error(struct error_state *estate)
{
	char *msg = peek(estate->error_stack);
	pop(estate->error_stack);
	printf(msg);
}