#include "include/eval.h"
#include "include/printer.h"
#include "include/reader.h"
#include <stdio.h>

int main(int args, char **argv) {

  InputBuffer * input_buffer = (InputBuffer *) new_input_buffer();
  
  while(1) {
    print_user_prompt();
    read(input_buffer);
    char * output = eval(input_buffer);
    print(output);
  }
  
  return 0;
}
