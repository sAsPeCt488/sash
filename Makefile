
CC = gcc
SRC = $(wildcard src/*.c)

sash:
		$(CC) $(SRC) -Wall -o sash.bin

clean:
		rm -f sash.bin
