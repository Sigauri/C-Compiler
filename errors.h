#include "lexer/stack.h"

struct error
{
	char *error_message;
};

struct error_state
{
	// Stack of errors to output
	struct stack *error_stack;

	int error;
};


void add_error(struct error_state *estate, char *msg);
void display_error(struct error_state *estate);
void init_error_state(struct error_state **estate);