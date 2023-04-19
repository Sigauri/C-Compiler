#include "stdio.h"
#include "../lexer/lex.h"
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "../lib/stack.h"
#include "../errors.h"

// Amount of synchronizing tokens
#define SYNC_TOKENS_QT 3


// What size should this be?
#define BT_BUFFER_SIZE 256


// Synchronizing tokens
const int SYNC_TOKENS[SYNC_TOKENS_QT] = {C_TOK_OPEN_BRACE, C_TOK_SEMI, C_TOK_UNKNOWN};

// lexer's state
extern struct c_lex_state c_lstate;

/* 
	TODOs:
		Organize error handling 
		
		Deal with freeing storage allocated for bt_state
		when its popped off the stack.
*/



// Parser's state
struct c_parser_state
{
	// current lookahead token
	struct c_token *lookahead;

	// Pointer to a function that gets the next token
	void (*move_lookahead)();

	// Stack of bt_state structs
	struct stack *st_bt_state;

	struct error_state *estate;

} *pstate = NULL;

/*	
	As soon as bt_start() is called, we need to start 
	storing tokens in bt_buffer[]. 
	
	When bt_restore() is called (if an error is encountered while parsing a production) 
	we need to stop storing tokens and start acquiring tokens from bt_buffer.
	If, while parsing we find out that there are no more tokens in bt_buffer[], 
	we must switch to lexer's strem and start storing tokens in bt_buffer[] again.
	Call bt_restore() again if given production can't be applied too. 

*/


// bactracking state (we have a stack of those - pstate->st_bt_state)
struct c_bt_state
{
	// Pointer to an array of pointers to tokens
	struct c_token **bt_buffer;	


	int backtracking;

	/* 
		These two are used when storing/reading
		into/from buffer, in  get_next_token
		and get_next_token_bt respectivly
	*/
	int buffer_read_id;

	int buffer_write_id;

	int _error

};



/*  
	A wrapper for lexer's c_lex_get_next_token()
	Allows to store token in pstate->lookahead 
	and store tokens in bt_buffer if bt_start
	is called.
*/  

void get_next_token()
{
	struct c_bt_state *bt_state = peek(pstate->st_bt_state);
	pstate->lookahead = c_lex_get_next_token();
	
	if(bt_state && bt_state->backtracking)
		bt_state->bt_buffer[bt_state->buffer_write_id++] = pstate->lookahead;
}


// Get the next token from bt_buffer
void get_next_token_bt()
{
	struct c_bt_state *bt_state = peek(pstate->st_bt_state);
	if(bt_state->buffer_read_id >= BT_BUFFER_SIZE) 
	{
		printf("%s\n", "Buffer exceeded");
		return;
	}
	if(!bt_state->bt_buffer[bt_state->buffer_read_id])
	{
		bt_state->backtracking = 0;
		bt_state->buffer_read_id = 0;
		pstate->move_lookahead = get_next_token;
		pstate->move_lookahead();
		return;
	}
	pstate->lookahead = bt_state->bt_buffer[bt_state->buffer_read_id++];
}



// Basic initialization stuff
void parser_init()
{
	pstate = malloc(sizeof(struct c_parser_state)); 

	pstate->move_lookahead = get_next_token;

	init_stack(&pstate->st_bt_state);

	init_error_state(&pstate->estate);
}

// Start bactracking
void  bt_start()
{
	struct c_bt_state *bt_state = malloc(sizeof(struct c_bt_state));

	// init bt_state
	bt_state->buffer_read_id = 0;
	bt_state->buffer_write_id = 0;
	bt_state->backtracking = 1;
	bt_state->bt_buffer = calloc(BT_BUFFER_SIZE, sizeof(struct c_token *));
	pstate->move_lookahead = get_next_token;

    bt_state->bt_buffer[bt_state->buffer_write_id++] = pstate->lookahead;
    bt_state->_error = 0;

	// push the state into stack of states
	push(pstate->st_bt_state, bt_state);


}

// Restore the state we had before bt_start was called
void bt_restore()
{
	struct c_bt_state *bt_state = peek(pstate->st_bt_state);
	bt_state->buffer_read_id = 0;
	pstate->move_lookahead = get_next_token_bt;
	pstate->move_lookahead();
}

// End backtracking
void bt_end()
{
	// free storage allocated for bt_state ?  

	pop(pstate->st_bt_state);

}

// #define bt_start() c_bt_state->backtracking = 1;


// #define bt_restore()							\
// 	c_bt_state->buffer_read_id = 0;					\
// 	pstate->move_lookahead = get_next_token_bt;	


void primary_expression();
void expression();
void postfix_expression();
void cast_expression();
void unary_expression();
void mul_expression();
void cond_expression();
void additive_expression();
void assignment_expression();


#define ERROR_ARGS_NUBER 6



/*
	
	We need to pass the message to error, so that we can
	take certain actions, and add the error to the error stack.

	If bactracking isn't active we need to output an error and
	skip the tokens til we meet one of the SYNC_TOKENS, otherwise
	we have to call add_error() to store the error in the stack of 
	errors and set the error variable to 1 ( So we know we must try another production)
*/

void error(char *msg, ...)
{	


	struct c_bt_state *bt_state = peek(pstate->st_bt_state);
	bt_state->_error++;
	printf("%s\n", "error() called");


	va_list valist;
	va_start(valist, msg);

	char *buffer = malloc(512);
	vsprintf(buffer, msg, valist);

	add_error(pstate->estate, buffer);

	if(!bt_state && !bt_state->backtracking)
	{
		
		int sync = 0;
		while(!sync)
		{
			for(int i = 0; i < SYNC_TOKENS_QT; i++)
			{
				if(pstate->lookahead->ttype == SYNC_TOKENS[i])
					sync = 1;
			}
			pstate->move_lookahead();
		}
	}

	pstate->estate->error++;
	va_end(valist);
		
	// if(bt_state->backtracking == 1)
	// {
	// 	bt_restore();
	// 	_error = 1;
	// 	return;
	// }
}

void expression()
{
	bt_start();
	assignment_expression();
	bt_end();
	//pstate->move_lookahead();
}

void primary_expression()
{
	if( pstate->lookahead->ttype != C_TOK_IDENTIFIER  &&
		pstate->lookahead->ttype != C_TOK_STRING		&&
		pstate->lookahead->ttype != C_TOK_CONSTANT	
	  )
	{
		//errors
		if(pstate->lookahead->ttype == C_TOK_OPEN_PAREN)
		{
			pstate->move_lookahead();
			expression();
			// pstate->move_lookahead();
			if(pstate->lookahead->ttype == C_TOK_CLOSE_PAREN) return;
			else error("Error at line %d: expression expected\n", c_lstate.line_num);
		}
		
		error("Error at line %d: expression expected\n", c_lstate.line_num);
	}
}


void unary_expression()
{

	switch(pstate->lookahead->ttype)
	{
		case C_TOK_PLUS_PLUS:
		{
			pstate->move_lookahead();
			unary_expression();
			break;
		}
		case C_TOK_MINUS_MINUS:
		{
			pstate->move_lookahead();
			unary_expression();
			break;
		}
		case C_TOK_BIT_AND: case C_TOK_MUL:
		case C_TOK_PLUS: case C_TOK_MINUS:
		case C_TOK_ONES_COMPLEMENT: case C_TOK_NOT:
		{
			pstate->move_lookahead();
			cast_expression();
			break;
		}
		default:
		{
			postfix_expression();
			// pstate->move_lookahead();
			break;
		}
		//sizeof
		//Alignof

	}

}

void cast_expression()
{
	unary_expression();
}

void mul_expression()
{
	int out = 0;
	cast_expression();
	while(!out)
	{
		switch(pstate->lookahead->ttype)
		{
			case C_TOK_MUL:
			{
				pstate->move_lookahead();
				cast_expression();
				continue;
				break;
			}

			case C_TOK_DIV:
			{
				pstate->move_lookahead();
				cast_expression();
				continue;			
				break;
			}

			case C_TOK_PERCENT:
			{
				pstate->move_lookahead();
				cast_expression();
				continue;
				break;
			}

			default:
			{
				out = 1;
			}
		}
	}
	
}

void additive_expression()
{
	int out = 0;
	mul_expression();
	while(!out)
	{
		switch(pstate->lookahead->ttype)
		{
			case C_TOK_PLUS:
			{
				pstate->move_lookahead();
				mul_expression();
				continue;
				break;
			}

			case C_TOK_MINUS:
			{
				pstate->move_lookahead();
				mul_expression();
				continue;			
				break;
			}

			default:
			{
				out = 1;
			}
		}
	}
	
}

void postfix_expression()
{

	primary_expression();
	int out = 0;
	while(!out)
	{

		pstate->move_lookahead();
		switch(pstate->lookahead->ttype)
		{
			case C_TOK_OPEN_SQR_BRACKET:
			{
				pstate->move_lookahead();
				expression();
				if(pstate->lookahead->ttype != C_TOK_CLOSE_SQR_BRACKET)
					error("error at line %d: expected postfix expression\n", c_lstate.line_num);
				continue;
				break;
			}
			case C_TOK_OPEN_PAREN:
			{
				//argument-expression-list
				break;

			}
			case C_TOK_DOT:
			{
				pstate->move_lookahead();
				if(pstate->lookahead->ttype != C_TOK_IDENTIFIER) 
					error("error at line %d: expected identifier\n", c_lstate.line_num);
						
				continue;
			}
			case C_TOK_ARROW:
			{
				pstate->move_lookahead();
				if(pstate->lookahead->ttype != C_TOK_IDENTIFIER) 
					error("error at line %d: expected identifier\n", c_lstate.line_num);
				continue;				
			}
			case C_TOK_PLUS_PLUS: case C_TOK_MINUS_MINUS: continue;

			default:
			{
				out = 1;
				continue;
			}
		}
	}

}

void shift_expression()
{
	int out = 0;
	additive_expression();
	while(!out)
	{
		switch(pstate->lookahead->ttype)
		{
			case C_TOK_SHIFT_L:
			{
				pstate->move_lookahead();
				additive_expression();
				continue;
				break;
			}

			case C_TOK_SHIFT_R:
			{
				pstate->move_lookahead();
				additive_expression();
				continue;			
				break;
			}

			default:
			{
				out = 1;
			}
		}
	}	
}

void relationl_expression()
{
	int out = 0;
	shift_expression();
	while(!out)
	{
		switch(pstate->lookahead->ttype)
		{
			case C_TOK_LESS:
			{
				pstate->move_lookahead();
				shift_expression();
				continue;
				break;
			}

			case C_TOK_MORE:
			{
				pstate->move_lookahead();
				shift_expression();
				continue;			
				break;
			}

			case C_TOK_LESS_EQ:
			{
				pstate->move_lookahead();
				shift_expression();
				continue;			
				break;				
			}

			case C_TOK_MORE_EQ:
			{
				pstate->move_lookahead();
				shift_expression();
				continue;			
				break;				
			}

			default:
			{
				out = 1;
			}
		}
	}		
}

void eq_expression()
{
	int out = 0;
	relationl_expression();
	while(!out)
	{
		switch(pstate->lookahead->ttype)
		{
			case C_TOK_EQ_EQ:
			{
				pstate->move_lookahead();
				relationl_expression();
				continue;
				break;
			}

			case C_TOK_NOT_EQ:
			{
				pstate->move_lookahead();
				relationl_expression();
				continue;			
				break;
			}

			default:
			{
				out = 1;
			}
		}
	}		
}

void and_expression()
{
	int out = 0;
	eq_expression();
	while(!out)
	{
		switch(pstate->lookahead->ttype)
		{
			case C_TOK_BIT_AND:
			{
				pstate->move_lookahead();
				eq_expression();
				continue;
				break;
			}

			default:
			{
				out = 1;
			}
		}
	}		
}

void excl_or_expression()
{
	and_expression();
	while(pstate->lookahead->ttype == C_TOK_EXP)
	{
		pstate->move_lookahead();
		and_expression();		
	}
}

void incl_or_expression()
{
	excl_or_expression();
	while(pstate->lookahead->ttype == C_TOK_BIT_OR)
	{
		pstate->move_lookahead();
		excl_or_expression();
	}
}

void logical_and_expression()
{
	incl_or_expression();
	while(pstate->lookahead->ttype == C_TOK_AND)
	{
		pstate->move_lookahead();
		incl_or_expression();
	}

}

void logical_or_expression()
{
	logical_and_expression();
	while(pstate->lookahead->ttype == C_TOK_OR)
	{
		pstate->move_lookahead();
		logical_and_expression();
	}

}


void cond_expression()
{
	logical_or_expression();
	while(pstate->lookahead->ttype == C_TOK_CONDITION)
	{
		pstate->move_lookahead();
		expression();
		if(pstate->lookahead->ttype != C_TOK_COLON)
			error("Error in line %d: symbol ':' expected in conditional expression", c_lstate.line_num);
		pstate->move_lookahead();
		cond_expression();
		printf("%s\n", "cond_expression parsed");
	}

}

void assignment_expression()
{

	if(pstate->lookahead->ttype != C_TOK_PLUS_PLUS && 
		pstate->lookahead->ttype != C_TOK_MINUS_MINUS &&
		pstate->lookahead->ttype != C_TOK_BIT_AND &&
		pstate->lookahead->ttype != C_TOK_MUL &&
		pstate->lookahead->ttype != C_TOK_PLUS &&
		pstate->lookahead->ttype != C_TOK_MINUS &&
		pstate->lookahead->ttype != C_TOK_ONES_COMPLEMENT &&
		pstate->lookahead->ttype != C_TOK_NOT &&
		pstate->lookahead->ttype != C_TOK_IDENTIFIER &&
		pstate->lookahead->ttype != C_TOK_CONSTANT &&
		pstate->lookahead->ttype != C_TOK_STRING &&
		pstate->lookahead->ttype != C_TOK_OPEN_PAREN)
	{
		printf("%s\n", "error");
		 return;
	}

	unary_expression();

	if(pstate->lookahead->ttype != C_TOK_EQ )
	{
		error("expected assignment operator");
	}
	pstate->move_lookahead();

	struct c_bt_state *bt_state = peek(pstate->st_bt_state);

	if(bt_state->_error)
	{
		int tmp = bt_state->_error;
		bt_restore();
		cond_expression();
		
		if(bt_state->_error > tmp)
		{
			display_error(pstate->estate);
			
		}
		return;
		bt_state->_error--;
		pstate->move_lookahead();	
	}

	
	
	bt_start();
	assignment_expression();
	bt_end();
	
	if(bt_state->_error) display_error(pstate->estate);

}