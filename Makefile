CFLAGS=-Wall -pedantic 
# -Werror
LIBS=-lpcre2-8
CC=gcc

olis: main.c ./src/* ./include/*
	$(CC) -o olis main.c src/* $(CFLAGS) $(LIBS)

run:
	./olis
