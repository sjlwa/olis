#define PCRE2_CODE_UNIT_WIDTH 8

#include "../include/reader.h"
#include <pcre2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

InputBuffer * new_input_buffer(void) {
  InputBuffer * input_buffer = malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->size = 0;
  input_buffer->length = 0;
  return input_buffer;
}

void free_input_buffer(InputBuffer * input_buffer) {
  free(input_buffer->buffer);
  free(input_buffer);
}

void read_input(InputBuffer * input_buffer) {
  
  ssize_t bytes_read = getline(&(input_buffer->buffer),
                              &(input_buffer->size),
                              stdin);
  // EOF after input
  if (feof(stdin)) exit(0);

  if (bytes_read == -1) {
    free_input_buffer(input_buffer);
    input_buffer = (InputBuffer *) new_input_buffer();

  } else {
    input_buffer->buffer[bytes_read - 1] = '\0';
    input_buffer->length = bytes_read - 1;
  }
    
}

TokenReader * new_reader(void) {
  TokenReader * reader = (TokenReader *) malloc(sizeof(TokenReader));
  reader->tokens = NULL;
  reader->position = 0;
  reader->length = 0;
  return reader;
}

void free_reader(TokenReader * reader) {
  free(reader->tokens);
  free(reader);
}

int reader_insert_token(TokenReader * reader, char * token) {
  reader->tokens = realloc(reader->tokens, sizeof(char *) * (reader->length + 1));
  if (reader->tokens == NULL) {
    free_reader(reader);
    return 1;
  }
  reader->tokens[reader->length] = token;
  reader->length++;
  return 0;
}

char * reader_next(TokenReader * reader) {
  return reader->tokens[reader->position++];
}

char * reader_peek(TokenReader * reader) {
  return reader->tokens[reader->position];
}

TokenReader * tokenize(InputBuffer * input_buffer) {

  TokenReader * reader = (TokenReader *) new_reader();

  pcre2_code *re;
  int error_number;
  PCRE2_SIZE error_offset;
  pcre2_match_data * match_data;
  int rc;
  PCRE2_SIZE *ovector;
  PCRE2_UCHAR *token_buffer = NULL;
  PCRE2_SPTR pattern = (PCRE2_SPTR) \
  "[\\s,]*(~@|[\\[\\]{}()'`~^@]|\"(?:\\\\.|[^\\\\\"])*\"?|;.*|[^\\s\\[\\]{}('\"`,;)]*)";
  
  re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &error_number, &error_offset, NULL);
  if (re == NULL) {
    PCRE2_UCHAR error_buffer[256];
    pcre2_get_error_message(error_number, error_buffer, sizeof(error_buffer));
    char * errmsg = "PCRE2 compilation failed at offset %d: %s";
    printf(errmsg, (int)error_offset, error_buffer);
    exit(1);
  }

  int start_offset = 0;
  for (;;) {
    if (start_offset != 0 && ovector[0]==ovector[1]) {
      break;
    }
    match_data = pcre2_match_data_create_from_pattern(re, NULL);
    rc = pcre2_match(re,
                    (PCRE2_SPTR)input_buffer->buffer,
                    (PCRE2_SIZE)input_buffer->length, 
                    start_offset, // start_offset 
                    0, 
                    match_data, 
                    NULL);

    if (rc < 0) {
      switch(rc) {
        case PCRE2_ERROR_NOMATCH: printf("No match\n");
          break;
        default: printf("Matching error %d\n", rc);
          break;
      }
    }

    // Extract substring
    PCRE2_SIZE bufflen = 0;
    pcre2_substring_get_bynumber(match_data, 1, &token_buffer, &bufflen);

    // Save token
    reader_insert_token(reader, (char*) token_buffer);
  
    // New start offset
    ovector = pcre2_get_ovector_pointer(match_data);
    start_offset = ovector[1];

  }

  pcre2_match_data_free(match_data);
  pcre2_code_free(re);
  pcre2_substring_free(token_buffer);

  return reader;

}

void list_insert(LispList * list, Lisp * lisp) {
  size_t new_size = sizeof(LispList) * list->length + sizeof(LispList);
  list->children = realloc(list->children, new_size);
  list->children[list->length] = lisp;
  list->length++;
}

LispList * new_list(void) {
  LispList * list = (LispList *) malloc(sizeof(LispList));
  list->children = NULL;
  list->length = 0;
  return list;
}

Lisp * read_buffer(InputBuffer * input_buffer) {
  TokenReader * token_reader = (TokenReader *) tokenize(input_buffer);
  return read_format(token_reader);
}

Lisp * read_list(TokenReader * reader) {
  
  reader_next(reader);
  
  // List grows dinamically as none right paren is found.
  LispList * list = new_list();

  //TODO: Stop if there's no more children. Missing closing seg fault
  
   while (1) {
     Lisp * lisp_child = read_format(reader);
    if (lisp_child->type == ATOM) {
      LispAtom * atom = (LispAtom *) lisp_child->data;

      if (atom->value[0] == ')') {
	free(lisp_child->data);
	free(lisp_child);
	break;
      }
    }
    list_insert(list, lisp_child);
  }
    
  // List wrapper
  Lisp * lisp_parent = (Lisp *) malloc(sizeof(Lisp));
  lisp_parent->data = (void *) list;
  lisp_parent->type = LIST;
  return lisp_parent;
}

Lisp * read_atom(TokenReader * reader) {
  LispAtom * atom = (LispAtom *) malloc(sizeof(LispAtom)); 
  atom->value = reader_next(reader);
 
  Lisp * lisp = (Lisp *) malloc(sizeof(Lisp));
  lisp->type = ATOM;
  lisp->data = (void *) atom;
 
  return lisp;
}

Lisp * read_format(TokenReader * reader) {
  char * token = reader_peek(reader);
  if (token[0] == '(') {
    return read_list(reader);
  } else {
    return read_atom(reader);
  }
}
