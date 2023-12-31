#define PCRE2_CODE_UNIT_WIDTH 8

#include "../include/reader.h"
#include <pcre2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void set_input_buffer(InputBuffer * input_buffer, char * buffer) {
  input_buffer->buffer = malloc(sizeof(char) * strlen(buffer) + 1);
  strcpy(input_buffer->buffer, buffer);
  input_buffer->size = strlen(buffer);
  input_buffer->length = strlen(buffer);
}

InputBuffer * new_input_buffer(void) {
  InputBuffer * input_buffer = malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;  
  input_buffer->size = 0;
  input_buffer->length = 0;
  return input_buffer;
}

void free_input_buffer(InputBuffer * input_buffer) {
  free(input_buffer->buffer);
  input_buffer->buffer = NULL;
  free(input_buffer);
}

int read_input(InputBuffer * input_buffer) {
  
  ssize_t bytes_read = getline(&(input_buffer->buffer),
                              &(input_buffer->size),
                              stdin);

  // EOF after input
  if (feof(stdin)) {
    free_input_buffer(input_buffer);
    input_buffer = NULL;
    exit(0);
  }

  // < 2 takes getline error and \n = 1, X\n = 2
  if (bytes_read < 2) {
    free_input_buffer(input_buffer);
    input_buffer = NULL;
    input_buffer = (InputBuffer *) new_input_buffer();
    return 1;

  } else {
    input_buffer->buffer[bytes_read - 1] = '\0';
    input_buffer->length = bytes_read - 1;
  }
  return 0;
}

TokenReader * new_reader(void) {
  TokenReader * reader = (TokenReader *) malloc(sizeof(TokenReader));
  reader->tokens = NULL;
  reader->position = 0;
  reader->length = 0;
  return reader;
}

void free_reader(TokenReader * reader) {
  if (reader != NULL) {
    for (size_t i = 0; i < reader->length; i++) {
      free(reader->tokens[i]);
      reader->tokens[i] = NULL;
    }
    free(reader->tokens);
    reader->tokens = NULL;
    free(reader);
  }
}

int reader_insert_token(TokenReader * reader, char * token) {  
  reader->tokens = realloc(reader->tokens, sizeof(char *) * (reader->length + 1));
  if (reader->tokens == NULL) {
    free_reader(reader);
    reader = NULL;
    return 1;
  }

  char *token_cpy = malloc(sizeof(char) * (strlen(token) + 1));
  strcpy(token_cpy, token);
  
  reader->tokens[reader->length] = token_cpy;
  reader->length++;
  return 0;
}

char * reader_next(TokenReader * reader) {
  //TODO:not to read when theres no more tokens available
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
  pcre2_match_data * match_data = NULL;
  int rc;
  PCRE2_SIZE * ovector = NULL;
  PCRE2_UCHAR * token_buffer = NULL;
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
  int left_edge = -2;
  int right_edge = -1;

  for (;;) {    
    if (ovector != NULL && start_offset != 0 && left_edge == right_edge) {
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
    left_edge = ovector[2];
    right_edge = ovector[3];

    //Free for every malloc
    pcre2_match_data_free(match_data);
    match_data = NULL;
    pcre2_substring_free(token_buffer);
    token_buffer = NULL;
  }
  
  pcre2_code_free(re);
  re = NULL;

  return reader;
}

void list_insert(LispList * list, Lisp * lisp) {
  size_t new_size = sizeof(Lisp*) * (list->length + 1);
  list->children = realloc(list->children, new_size);
  if (list->children != NULL) {
    list->children[list->length] = lisp;
    list->length++;
  } else {
    fprintf(stderr, "Error: list realloc\n");
    exit(1);
  }
}

LispList * new_list(void) {
  LispList * list = (LispList *) malloc(sizeof(LispList));
  list->children = NULL;
  list->length = 0;
  return list;
}
 
void free_list(LispList * list) {
  if (list == NULL) return;
  for (int i=0; i<list->length; i++) {
    free_lisp(list->children[i]);
    list->children[i] = NULL;
  }
  
  free(list->children);
  list->children = NULL;
  
  free(list);
}

Lisp * read_buffer(InputBuffer * input_buffer) {
  TokenReader * token_reader = (TokenReader *) tokenize(input_buffer);
  Lisp * lisp = read_format(token_reader);
  free_reader(token_reader);
  token_reader = NULL;
  return lisp;
}

Lisp * new_lisp(LispType type, void * data) {
  Lisp * lisp = (Lisp *) malloc(sizeof(Lisp));
  if (lisp==NULL) {
    perror("Allocation error: new_lisp");
    exit(EXIT_FAILURE);
  }
  lisp->type = type;
  lisp->data = data;
  return lisp;
}

void free_lisp(Lisp * lisp) {
  if (lisp == NULL) return;
  
  switch (lisp->type) {
    case ATOM: {
      free_atom(lisp->data);
      break;
    }
    case LIST: {
      free_list(lisp->data);
      break;
    }
    case SYMBOL: {
      free_symbol(lisp->data);
      break;
    }
    default: break;
  }
  lisp->data = NULL;
  free(lisp);
}

LispAtom * new_atom(char * value) {
  LispAtom * atom = (LispAtom *) malloc(sizeof(LispAtom));
  atom->value = strdup(value);
  return atom;
}

void free_atom(LispAtom * atom) {
  if (atom == NULL) return;
  free(atom->value);
  atom->value = NULL;
  free(atom);
}


LispSymbol * new_symbol(char * value) {
  LispSymbol * symbol = (LispSymbol *) malloc(sizeof(LispSymbol));
  symbol->value = strdup(value);
  return symbol;
}

void free_symbol(LispSymbol * symbol) {
  if (symbol == NULL) return;
  free(symbol->value);
  symbol->value = NULL;
  free(symbol);
}

Lisp * read_list(TokenReader * reader) {
  
  reader_next(reader);
  
  // List grows dinamically as none right paren is found.
  LispList * list = new_list();

  //TODO: Stop if there's no more children and a missing right parenthesis (seg fault)
   while (1) {
     Lisp * lisp_child = read_format(reader);
     if (lisp_child->type == ATOM) {       
      LispAtom * atom = (LispAtom *) lisp_child->data;
      if (atom->value[0] == ')') {
        free_lisp(lisp_child);
	lisp_child = NULL;
      	break;
      }
    }
    list_insert(list, lisp_child);
  }
    
  // List wrapper
  Lisp * lisp_parent = new_lisp(LIST, (void *) list);
  return lisp_parent;
}

Lisp * read_atom(TokenReader * reader) {
  LispAtom * atom = new_atom(reader_next(reader));
  Lisp * lisp = new_lisp(ATOM, (void *) atom);
  return lisp;
}

Lisp * read_symbol(TokenReader * reader) {
  LispSymbol * symbol = new_symbol(reader_next(reader));
  Lisp * lisp = new_lisp(SYMBOL, (void *) symbol);
  return lisp;
}

Lisp * read_format(TokenReader * reader) {
  char * token = reader_peek(reader);
  
  if (strlen(token) == 1) {
    switch(token[0]) {
      case '(': return read_list(reader);
      case '+': return read_symbol(reader);
      default: break;
    }
  }
  if (strcmp(token, "exit") == 0) {
    return read_symbol(reader);
  }

  return read_atom(reader);
}
