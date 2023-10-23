#ifndef EVAL_H
#define EVAL_H

#include "../include/types.h"
#include "../include/reader.h"

char * build_from_token_reader(TokenReader * reader);

char * build_from_lisp(Lisp * lisp);
void __build_from_lisp(Lisp * lisp,
			 char ** out,
			 size_t * out_len,
			 _LispMeta * meta);

Lisp * eval(Lisp * lisp);
Lisp * eval_ast(Lisp * lisp);

#endif // EVAL_H
