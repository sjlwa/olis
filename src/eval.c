#include "../include/buffer.h"
#include "../include/reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char * eval_token_reader(TokenReader * reader) {
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


char * __eval(Lisp * lisp, char * out, size_t * out_len) {

  if (out_len == NULL) {
    out_len = malloc(sizeof(size_t));
    *out_len = 0;
  }
  
  switch(lisp->type) {
    case ATOM: {
      LispAtom * atom = (LispAtom *) lisp->data;
      size_t value_len = strlen(atom->value);

      out = realloc(out, *out_len + value_len + 2);
      memcpy(&out[*out_len], atom->value, value_len);
      *out_len += value_len;
      out[*out_len] = ' ';
      out[++(*out_len)] = '\0';
      
      return out;
    }
      
    case LIST: {
      LispList * list = (LispList *) lisp->data;

      out = realloc(out, *out_len + 2);
      out[*out_len] = '(';
      out[++(*out_len)] = '\0';

      for (int i=0; i<list->length; ++i) {
	Lisp * lisp = list->children[i];
	__eval(lisp, out, out_len);
      }

      out[*out_len - 1] = ')';
      (*out_len)++;
     
      return out;
    }
  }
  
  return "";
}
