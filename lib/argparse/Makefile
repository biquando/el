CC = gcc
AR = ar

BUILD_FLAGS = -Wall -pedantic -ansi -I./include
TEST_FLAGS = -I./include ./*.a

SRC = $(wildcard ./src/*.c)
OBJ = $(SRC:.c=.o)
TEST_FILES = $(wildcard ./tests/*.c)

TEST_ARGS = hello.c --quiet -o b.out bye.c -n 1 2 3 -h

.PHONY: all libs test clean

all: $(OBJ)
	$(AR) -crv libargparse.a $(OBJ)

%.o: %.c
	$(CC) -o $@ -c $< $(BUILD_FLAGS)

libs:
	make -C lib/vector
	cp ./lib/*/*.a .

test: all
	mkdir -p ./bin
	$(CC) -o ./bin/testargp $(TEST_FILES) $(TEST_FLAGS)
	./bin/testargp $(TEST_ARGS)

clean:
	rm -f ./*.a $(OBJ)
	rm -rf ./bin
	-make -C lib/vector clean
