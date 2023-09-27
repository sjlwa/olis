#include "../include/reader.h"
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

  if (bytes_read == -1) {
    free_input_buffer(input_buffer);
    input_buffer = (InputBuffer *) new_input_buffer();

  } else {
    input_buffer->buffer[bytes_read - 1] = '\0';
    input_buffer->length = bytes_read - 1;
  }
    
}
