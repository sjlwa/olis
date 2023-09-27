#include "../include/buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pre_eval(char * buffer) {
  if (feof(stdin)) {
    exit(0);

  } else if (strcmp(buffer, "exit") == 0) {
    exit(0);
  }
}

char * eval(InputBuffer * input_buffer) {

  pre_eval(input_buffer->buffer);

  return input_buffer->buffer;
}
