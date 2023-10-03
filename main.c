#include "include/eval.h"
#include "include/printer.h"
#include "include/reader.h"
#include <stdio.h>

int main(int args, char **argv) {

  InputBuffer * input_buffer = (InputBuffer *) new_input_buffer();
  
  while(1) {
    print_user_prompt();
    read_input(input_buffer);
    Lisp * lisp = read_buffer(input_buffer);
    char * output = eval(lisp);
    print(output);
  }
  
  return 0;
}
