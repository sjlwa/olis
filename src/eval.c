#include "../include/buffer.h"
#include "../include/reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char * eval(TokenReader * reader) {

  char * output = malloc(sizeof(char));
  int output_length = 0;

  char * token;
  while (reader->position < reader->length) {
    token = reader_next(reader);
  
    output = realloc(output, output_length + strlen(token) + 2);
    mempcpy(&output[output_length], token, strlen(token));
    output_length += strlen(token);

    output[output_length] = ' ';
    output_length++;
    output[output_length] = '\0';
  }

  return output;
}
