prog: ./parser/parser.c lexer/lex.c lexer/lex_default.c lib/hashtable.c lib/stack.c errors.c
	gcc	lib/stack.c  lexer/lex.c lexer/lex_default.c lib/hashtable.c errors.c ./parser/parser.c -o c-comp -g  
