#define _OPEN_SYS_ITOA_EXT

#include "../include/types.h"
#include "../include/reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char * build_from_token_reader(TokenReader * reader) {
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



char * __build_from_lisp(Lisp * lisp, char * out, size_t * out_len) {

  if (out_len == NULL) {
    out_len = malloc(sizeof(size_t));
    *out_len = 0;
  }

  if (out == NULL) {
    out = malloc(sizeof(char));
    out[*out_len] = '\0';
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

    case SYMBOL: {
      LispSymbol * symbol = (LispSymbol *) lisp->data;
      size_t value_len = strlen(symbol->value);

      out = realloc(out, *out_len + value_len + 2);
      memcpy(&out[*out_len], symbol->value, value_len);
      *out_len += value_len;
      out[*out_len] = ' ';
      out[++(*out_len)] = '\0';
      
      return out;
    }
      
    // TODO: (FIX) Bad reading of a list 
    // example.  ( ( () ) ( ()()()() ) ) ...
    // () -> )
    case LIST: {
      LispList * list = (LispList *) lisp->data;

      out = realloc(out, *out_len + 2);
      out[*out_len] = '(';
      out[++(*out_len)] = '\0';

      for (int i=0; i<list->length; ++i) {
        Lisp * lisp = list->children[i];
        __build_from_lisp(lisp, out, out_len);
      }

      out[*out_len - 1] = ')';
      (*out_len)++;
     
      return out;
    }
  }
  
  return "";
}


Lisp * __exit(Lisp * a, Lisp * b) {
  printf("exit 0");
  return NULL;
}


#define __bin_op_precall \
  LispAtom * a1 = a->data; \
  LispAtom * b1 = b->data; \
  int ai = atoi(a1->value); \
  int bi = atoi(b1->value);

// TODO: dynamic value_buffer size
#define __bin_op_postcall \
  char value_buffer[64]; \
  snprintf(value_buffer, 64, "%d", r); \
  return new_lisp(ATOM, (void *) new_atom(value_buffer)); \


Lisp * __add(Lisp * a, Lisp * b) {
  __bin_op_precall
  int r = ai + bi;
  __bin_op_postcall
}

// Lisp * __sub(Lisp * a, Lisp * b) { return a - b; }
/* Lisp * __mul(Lisp * a, Lisp * b) { return a * b; } */
/* Lisp * __div(Lisp * a, Lisp * b) { return a / b; } */
/* Lisp * __mod(Lisp * a, Lisp * b) { return a % b; } */
//int __undefined(int a, int b) { exit(1); }

symfunc repl_env(char * sym) {
  if (strlen(sym) == 1) {
    switch (sym[0]) {
      case '+': return &__add;
      // case '-': return &__sub;
      // case '*': return &__mul;
      // case '/': return &__div;
      // case '%': return &__mod;
    }
  }

  if (strcmp(sym, "exit") == 0) {
    return &__exit;
  }
  
  // TODO: free on error
  fprintf(stdout, "Error: Undefined Symbol %s\n", sym);
  exit(1);
}

Lisp * eval_ast(Lisp * lisp) {
  switch(lisp->type) {

    case ATOM: {
      return lisp;
    }

    case LIST: {
      LispList * list = (LispList *) lisp->data;

      if (list->length == 0) {
        free_lisp(lisp);
	lisp = NULL;
        return new_lisp(ATOM, new_atom("null"));
      }

      // Saves elements in an executable format
      for (int i=0; i<list->length; i++) {
        list->children[i] = eval_ast(list->children[i]);
      }

      if (list->children[0]->type == SYMBOL) {
        LispSymbol * symbol = list->children[0]->data;
      
        // TODO: Fix void lisp when asigning same expression more than once
        // TODO: Fix void lisp when list have more than 3 elements

        Lisp * accumulator = new_lisp(ATOM, new_atom("0"));


        //Execute the assigned method
        for (int i=1; i<list->length; i++) {
          Lisp * lisp_el = list->children[i];

          accumulator = symbol->func(accumulator, lisp_el);
          if (accumulator == NULL) {
            free_lisp(lisp);
	    lisp = NULL;
            // // TODO: set causes of exit
          }

        }
        
        return accumulator;
      }
      return lisp;
    }

    case SYMBOL: {
      LispSymbol * symbol = (LispSymbol *) lisp->data;
      symbol->func = repl_env(symbol->value);
      return lisp;
    }
  }

  return new_lisp(ATOM, (void *) new_atom(""));
}

Lisp * eval(Lisp * lisp) {
  return eval_ast(lisp);
}
