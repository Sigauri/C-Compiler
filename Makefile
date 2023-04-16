prog: parser.c lexer/lex.c lexer/lex_default.c lexer/hashtable.c lexer/stack.c errors.c
	gcc	lexer/stack.c  lexer/lex.c lexer/lex_default.c lexer/hashtable.c errors.c parser.c -o prog -g  
