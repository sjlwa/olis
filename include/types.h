#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>

typedef struct {
  char * buffer;
  size_t size;
  ssize_t length;
} InputBuffer;

typedef struct {
  char ** tokens;
  size_t position;
  size_t length;
} TokenReader;

typedef enum {
  LIST,
  ATOM,
  SYMBOL
} LispType;

typedef struct {
  LispType type;
  void * data;
} Lisp;

typedef struct {
  Lisp ** children;
  size_t length;
} LispList;

typedef struct {
  char * value;
} LispAtom;

typedef Lisp * (*symfunc)(Lisp *, Lisp *);

typedef struct {
  char * value;
  symfunc func;
} LispSymbol;


typedef struct {
  size_t pointer;
  size_t length;
} _ListMeta;

typedef struct {
  _ListMeta list;
} _LispMeta;
  
#endif // TYPES_H
