#include "stdio.h"
#include "../lexer/lex.h"
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "../lib/stack.h"
#include "../errors.h"

#define SYNC_TOKENS_QT 3

#define BT_BUFFER_SIZE 256

#define ERROR_ARGS_NUBER 6

const int SYNC_TOKENS[SYNC_TOKENS_QT] = {C_TOK_OPEN_BRACE, C_TOK_SEMI, C_TOK_UNKNOWN};

extern struct c_lex_state c_lstate;

struct c_parser_state;

struct c_bt_state;

void get_next_token();

void get_next_token_bt();

void parser_init();

void bt_start();

void bt_restore();

void bt_end();

void primary_expression();
void expression();
void postfix_expression();
void cast_expression();
void unary_expression();
void mul_expression();
void cond_expression();
void additive_expression();
void assignment_expression();

void error(char *msg, ...);

void expression();

void primary_expression();

void unary_expression();

void cast_expression();

void mul_expression();

void additive_expression();

void postfix_expression();

void shift_expression();

void relational_expression();

void eq_expression();

void and_expression();

void excl_or_expression();

void incl_or_expression();

void logical_and_expression();

void logical_or_expression();

void cond_expression();

void assignment_expression();