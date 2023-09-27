#ifndef READER_H
#define READER_H

#include "buffer.h"

InputBuffer * new_input_buffer(void);
void read(InputBuffer * input_buffer);

typedef struct {
  char ** tokens;
  size_t position;
  size_t length;
} TokenReader;

TokenReader * new_reader(void);
void free_reader(TokenReader * reader);
int reader_insert_token(TokenReader * reader, char * token);
char * reader_next(TokenReader * reader);
char * reader_peek(TokenReader * reader);
TokenReader * tokenize(InputBuffer * input_buffer);

#endif // READER_H
