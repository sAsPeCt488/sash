
CC = gcc
SRC = $(wildcard src/*/*.c)

sash:
		$(CC) src/main.c $(SRC) -Wall -o sash.bin -I src

clean:
		rm -f sash.bin