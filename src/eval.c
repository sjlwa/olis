#define _OPEN_SYS_ITOA_EXT

#include "../include/eval.h"
#include "../include/types.h"
#include "../include/reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


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


char * build_from_lisp(Lisp * lisp) {
  char * output = malloc(1);
  output[0] = '\0';
  size_t output_length = 0;
  __build_from_lisp(lisp, &output, &output_length, NULL);
  return output;
}

void __build_from_lisp(Lisp * lisp, char ** out, size_t * out_len, _LispMeta * meta) {
 
  int is_last_elem = meta != NULL && meta->list.pointer == meta->list.length - 1;
  
  switch(lisp->type) {
    case ATOM: {
      LispAtom * atom = (LispAtom *) lisp->data;
      size_t value_len = strlen(atom->value);
      
      if (meta==NULL || is_last_elem) {
	*out = realloc(*out, *out_len + value_len + 1);
	memcpy(*out + *out_len, atom->value, value_len);
	*out_len += value_len;
	(*out)[*out_len] = '\0';

      } else {
	*out = realloc(*out, *out_len + value_len + 2);
	memcpy(*out + *out_len, atom->value, value_len);
	*out_len += value_len;
	(*out)[*out_len] = ' ';
	(*out)[++(*out_len)] = '\0';
      }
      break;
    }

    case SYMBOL: {
      LispSymbol * symbol = (LispSymbol *) lisp->data;
      size_t value_len = strlen(symbol->value);

      if (meta==NULL || is_last_elem) {
	*out = realloc(*out, *out_len + value_len + 1);
	memcpy(*out + *out_len, symbol->value, value_len);
	*out_len += value_len;
	(*out)[*out_len] = '\0';
	
      } else {
	*out = realloc(*out, *out_len + value_len + 2);
	memcpy(*out + *out_len, symbol->value, value_len);
	*out_len += value_len;
	(*out)[*out_len] = ' ';
	(*out)[++(*out_len)] = '\0';
      }
      break;
    }
      
    case LIST: {
      LispList * list = (LispList *) lisp->data;

      *out = realloc(*out, *out_len + 2);
      (*out)[*out_len] = '(';
      (*out)[++(*out_len)] = '\0';

      for (int i=0; i<list->length; ++i) {
        Lisp * lisp = list->children[i];

	// Points to current for index based processing. (Last element, ...)
	_ListMeta lm = {.length = list->length, .pointer = i };
	_LispMeta lisp_meta = {.list = lm};
	
        __build_from_lisp(lisp, out, out_len, &lisp_meta);
      }

      *out = realloc(*out, *out_len + 2);
      (*out)[*out_len] = ')';
      (*out)[++(*out_len)] = '\0';
     
      break;
    }
  }
  
}


Lisp * __exit(Lisp * a, Lisp * b) {
  printf("exit 0");
  return NULL;
}


#define __bin_op_precall \
  LispAtom * a1 = a->data; \
  LispAtom * b1 = b->data; \
  int ai = atoi(a1->value); \
  int bi = atoi(b1->value); \
  free_lisp(a); a = NULL; // Acumulator is deallocated

// TODO: dynamic value_buffer size
#define __bin_op_postcall \
  char value_buffer[64]; \
  snprintf(value_buffer, 64, "%d", r); \
  Lisp * result = new_lisp(ATOM, (void *) new_atom(value_buffer)); \
  return result; \


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
      
        Lisp * accumulator = new_lisp(ATOM, new_atom("0"));

        //Execute the assigned method
        for (int i=1; i<list->length; i++) {
          Lisp * lisp_el = list->children[i];
          accumulator = symbol->func(accumulator, lisp_el);	  
        }
	
	free_lisp(lisp);
	lisp = NULL;

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
