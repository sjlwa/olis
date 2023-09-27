CFLAGS=-Wall -Werror -pedantic
CC=gcc

olis: main.c src/*
	$(CC) -o olis main.c src/* $(CFLAGS)
