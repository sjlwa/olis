#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../include/types.h"
#include "../include/reader.h"
#include "../include/eval.h"

InputBuffer * input_buffer = NULL;
Lisp * lisp = NULL;
char * output = NULL;

void setup(void) {
    input_buffer = new_input_buffer();
}

void teardown(void) {
  free_input_buffer(input_buffer);
  input_buffer = NULL;
  free_lisp(lisp);
  lisp = NULL;
  free(output);
}

Test(tokenize_and_build, test_token_reading, .init=setup) {
  set_input_buffer(input_buffer, "   (   )        ");
  lisp = read_buffer(input_buffer);
  output = build_from_lisp(lisp);   
  cr_assert(eq(str, output, "()"));
}
