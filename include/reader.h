#ifndef READER_H
#define READER_H

#include "buffer.h"

InputBuffer * new_input_buffer(void);
void read_input(InputBuffer * input_buffer);

typedef struct {
  char ** tokens;
  size_t position;
  size_t length;
} TokenReader;

typedef enum {
  LIST, ATOM
} LispType;

typedef struct {
  LispType type;
  void * data;
} Lisp;

typedef struct {
  Lisp ** children;
  size_t length;
} LispList;

typedef struct {
  char * value;
} LispAtom;

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
LispList * new_list(void);
void list_insert(LispList * list, Lisp * atom);


#endif // READER_H
