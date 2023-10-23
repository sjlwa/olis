#include "../include/types.h"
#include "../include/reader.h"
#include "../include/eval.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int args, char **argv) {

  if (args > 1) {
   
    if (strcmp(argv[1], "tokenize") == 0) {
      
      InputBuffer * input_buffer = new_input_buffer();
      set_input_buffer(input_buffer, "()");;
      TokenReader * token_reader = tokenize(input_buffer);
      free_reader(token_reader);
      free_input_buffer(input_buffer);
      
    } else if (strcmp(argv[1], "eval") == 0) {

      InputBuffer * input_buffer = new_input_buffer();
      //set_input_buffer(input_buffer, "(+ 1 1)");
      //set_input_buffer(input_buffer, "( ()  (()()()()) )");
      set_input_buffer(input_buffer, "( ( () ) (  ( )  ) ) ");
      
      Lisp * lisp = read_buffer(input_buffer);
      lisp = eval(lisp);
      
      char * output = build_from_lisp(lisp);
      printf("output: %s\n", output);

      free(output);
      free_input_buffer(input_buffer);
      free_lisp(lisp);
    }
    
  }
  
}
