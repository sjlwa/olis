#ifndef READER_H
#define READER_H

#include "types.h"

InputBuffer * new_input_buffer(void);
int read_input(InputBuffer * input_buffer);

TokenReader * new_reader(void);
void free_reader(TokenReader * reader);
int reader_insert_token(TokenReader * reader, char * token);
char * reader_next(TokenReader * reader);
char * reader_peek(TokenReader * reader);
TokenReader * tokenize(InputBuffer * input_buffer);

Lisp * read_buffer(InputBuffer * input_buffer);
Lisp * read_list(TokenReader * reader);
Lisp * read_atom(TokenReader * reader);
Lisp * read_format(TokenReader * reader);

LispAtom * new_atom(char * value);
LispSymbol * new_symbol(char * value);
LispList * new_list(void);
Lisp * new_lisp(LispType type, void * data);

void free_atom(LispAtom * atom);
void free_lisp(Lisp * lisp);
void free_symbol(LispSymbol * symbol);
void free_list(LispList * list);

void list_insert(LispList * list, Lisp * atom);


#endif // READER_H
