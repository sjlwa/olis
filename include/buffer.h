#ifndef BUFFER_H
#define BUFFER_H

#include <sys/types.h>

typedef struct {
  char * buffer;
  size_t size;
  ssize_t length;
} InputBuffer;

#endif // BUFFER_H
