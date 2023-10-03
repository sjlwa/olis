#ifndef EVAL_H
#define EVAL_H

#include "../include/buffer.h"
#include "../include/reader.h"

char * eval_token_reader(TokenReader * reader);
char * __eval(Lisp * lisp, char ** out_addr, size_t * out_len);
#define eval(lisp) __eval(lisp, NULL, NULL);

#endif // EVAL_H
