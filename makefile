all: parsec

parsec: main.c
	gcc -g -fsanitize=address main.c -o parsec
