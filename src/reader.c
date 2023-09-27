#define PCRE2_CODE_UNIT_WIDTH 8

#include "../include/reader.h"
#include <pcre2.h>
#include <stdio.h>
#include <stdlib.h>


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

void read(InputBuffer * input_buffer) {
  
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
