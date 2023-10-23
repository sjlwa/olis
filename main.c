#include "include/eval.h"
#include "include/printer.h"
#include "include/reader.h"
#include <stdio.h>
#include <stdlib.h>

#include "include/types.h"

int main(int args, char **argv) {

  InputBuffer * input_buffer = (InputBuffer *) new_input_buffer();
  
   while(1) {
    print_user_prompt();
    if (read_input(input_buffer) == 1) continue;
  
    Lisp * lisp = read_buffer(input_buffer);
    lisp = eval(lisp);
    char * output = build_from_lisp(lisp);
    print_output(output);
    
    free(output);
    output = NULL;

    free_lisp(lisp);
    lisp = NULL;
  }

  free_input_buffer(input_buffer);
  input_buffer = NULL;

  return 0;
}
