CFLAGS=-Wall -pedantic -g
# -Werror
LIBS=-lpcre2-8
LIBS_TEST=-lcriterion
CC=gcc

olis: main.c ./src/* ./include/*
	$(CC) -o olis main.c src/* $(CFLAGS) $(LIBS)

test: tests/tests.c ./src/* ./include/*
	$(CC) -o tests/olis_tests tests/tests.c src/* $(CFLAGS) $(LIBS) $(LIBS_TEST)
	./tests/olis_tests

memtest: tests/mem/* src/* include/*
	$(CC) -o tests/mem/tokenize tests/mem/*.c src/*.c $(CFLAGS) $(LIBS) $(LIBS_TEST)

run_memtest:
	valgrind ./tests/mem/tokenize


run:
	./olis
