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

memtest: tests/memtest.c src/* include/*
	$(CC) -o tests/memtest tests/memtest.c src/*.c $(CFLAGS) $(LIBS) $(LIBS_TEST)

run_memtest:
	valgrind ./tests/memtest


run:
	./olis
