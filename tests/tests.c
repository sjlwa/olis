#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdio.h>

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

  set_input_buffer(input_buffer, "   (  ())        ");
  lisp = read_buffer(input_buffer);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "(())"));

  set_input_buffer(input_buffer, "   (  ( ()   ))        ");
  lisp = read_buffer(input_buffer);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "((()))"));

  set_input_buffer(input_buffer, "( ( () ) ( ()()()() ) )");
  lisp = read_buffer(input_buffer);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "((())(()()()()))"));

  set_input_buffer(input_buffer, "( ( () ) ( ()()()() ) )");
  lisp = read_buffer(input_buffer);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "((())(()()()()))"));
}

Test(eval, test_evaluation, .init=setup) {
  set_input_buffer(input_buffer, "   (  ( ()   ))        ");
  lisp = read_buffer(input_buffer);
  lisp = eval(lisp);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "((null))"));

  set_input_buffer(input_buffer, "( ()  (()()()()) )");
  lisp = read_buffer(input_buffer);
  lisp = eval(lisp);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "(null (null null null null))"));

  set_input_buffer(input_buffer, "( ( () ) (  ( )  ) ) ");
  lisp = read_buffer(input_buffer);
  lisp = eval(lisp);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "((null) (null))"));
   
  set_input_buffer(input_buffer, "(+ 1 1)");
  lisp = read_buffer(input_buffer);
  lisp = eval(lisp);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "2"));

  set_input_buffer(input_buffer, "(+ 1 2 3 4 (+ 1 2 3 4 (+ 1 2 3 4)))");
  lisp = read_buffer(input_buffer);
  lisp = eval(lisp);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "30"));
  
  set_input_buffer(input_buffer, "(+ (+ (+ 1 2 3 4) 1 2 3 4) 1 2 3 4)");
  lisp = read_buffer(input_buffer);
  lisp = eval(lisp);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "30"));

  set_input_buffer(input_buffer, " ( + (+ 1 2 3 4 5) (+ 1 2 3 4 (+ 1 2 3 4)))");
  lisp = read_buffer(input_buffer);
  lisp = eval(lisp);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "35"));

  set_input_buffer(input_buffer, "((+ 1 2 3 4 5) (+ 1 2 3 4 (+ 1 2 3 4)))");
  lisp = read_buffer(input_buffer);
  lisp = eval(lisp);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "(15 20)"));

  set_input_buffer(input_buffer, "( ( (1) ) ( + (1)(1)(1)(1) ) )");
  lisp = read_buffer(input_buffer);
  lisp = eval(lisp);
  output = build_from_lisp(lisp);
  cr_assert(eq(str, output, "Error"));
}
