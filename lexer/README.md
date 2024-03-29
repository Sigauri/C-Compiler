# Lexical analyzer design

## What does it do? 

The lexer takes a stream of characters from a file and forms a structure called "token".
The structure contains the type of the token and its lexeme. 


## How does it work ? 

Before using any functions of the lexer, the user must call the initialization function,
which initializes standard search paths for header files, the lexer state etc.

The user gets tokens by calling get_next_token(), which returns a pointer to c_token data structure.


## Keywords and Identifiers 

Identifier and keywords consist of characters, digits and the underscore characters, but always start 
with a character.


## Reading Files  

The lexer will read the file to be lexed with C standard lib functions 
and store data into a buffer. All dynamic memory allocation will be done with malloc.
A data structure containing infos about the file will be malloce'd for each
file that was opened.


## Recognition and Handling of punctuators 

The full list of punctuators:
>-<+/*%!&|^#.=[](){},;:~

First 14 characters can be continued with multiple characters, for example:

>	>>	>>= >=


## Symbol Table 

We need symbol table to store some additive info
for the IDs and Keywords, to check if the variables are declared etc. 
The info is to be used by the parser, code generator etc. 

The minimum info to be assosicated with ID/Keyword: 
-first occurence
-its lexeme
-its type

How will token's position be present?
It'll be a structure containing following fields:
-Offset into buffer
-Number of the line
-Offset from the start of the line

Basic design

A new symbol table is created for each declaration scope.
Every ST contains a "prev", which points to the ST 
for the previous declaration scope. The very first ST is 
for global names, its "prev" field is always NULL.


-Implementation
Symbol table will be implemented as a hash table,
where each element contains a pointer to data we need
and a pointer to next element(Needed to handle collisions).
If we make a lookup in a table and NULL is returned,
it means this ID/kwd is not in a table.

The hash table is to be implemented as an array of
linked lists. Each element in list consists 
of pointer to data and next element.
The hash table size is always a power of two. 
Every time the load factor drops under 0.25
or exceeds 0.75 the hash table will be resized.

Collision resolution method - Separate chaining.




-Basic interface

-HASH TABLE

get_hash(str) 		- Get hash code for str
ht_create() 		- Create a hash table
ht_destroy()		- Remove a hash table and free storage
ht_insert(id, node) - Insert node at ht[id]
ht_remove(id)		- Remove node at ht[id]
ht_lookup(id)		- Get node at ht[id]

-SYMBOL TABLE

Symbol table

A new symbol table is created for each declaration scope.
Every ST contains a "prev" field, which points to the ST 
for the previous declaration scope. The very first ST is 
for global names, its "prev" field is always NULL.

If a lookup in the current ST failed, we look in prev
STs. 

Some basic stuff to implement

st_create() - create symbol table
st_destroy(st) - remove symbol table st, free all the memory allocated

st_add(st, entry) 	- add an entry to the symbol table st
st_get(st, value)	- get an entry from st 
st_remove(st, value) - remove an entry from the symbol table st

/////////////////////////////////////////////////////////


/// Main structures ///
-Lexer state 
-Token
-The file structure


/// DONE ///

-Input Bufferisation
-Basic data structures
-Basic initialization
-Stack of header files

/// TODO ///

-Symbol table
-Comment everything.



/// Thoughts ///
-Encoding
-Symbol table - global and local.
-#include directive handling
-Keep track of new line chars
-Error handling? Warning messages?
-The interface for using the lexer ?
-Hash table.
 How big should it be?
 Expand when too many collisions?
 Iterating through whole array when destroying ht?
 Can't be used for every type of data and key.
 
 


