CC = gcc
AR = ar
BUILD_FLAGS = -Wall -pedantic -ansi -I./include
TEST_FLAGS = -I./include -L. -lvector

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TEST_FILES = $(wildcard tests/*.c)

.PHONY: all test clean

all: $(OBJ)
	$(AR) -crv ./libvector.a $(OBJ)

%.o: %.c
	$(CC) -o $@ -c $< $(BUILD_FLAGS)

test: all
	mkdir -p ./bin
	$(CC) -o ./bin/vectest $(TEST_FILES) $(TEST_FLAGS)
	./bin/vectest

clean:
	rm -f $(OBJ) ./libvector.a
	rm -rf ./bin
