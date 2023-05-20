.PHONY:all clean

all: database
CC=riscv64-unknown-elf-gcc

main.o: main.c
	$(CC) -c main.c -o main.o

database: main.o
	$(CC) main.o -g -Wall -o database


clean:
	rm database main.o -rf
