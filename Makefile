CFLAGS=-Wall -Werror -pedantic
LIBS=-lpcre2-8
CC=gcc

olis: main.c src/*
	$(CC) -o olis main.c src/* $(CFLAGS) $(LIBS)
