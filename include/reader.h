#ifndef READER_H
#define READER_H

#include "buffer.h"

InputBuffer * new_input_buffer(void);
void read(InputBuffer * input_buffer);

#endif // READER_H
