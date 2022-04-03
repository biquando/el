CC = gcc

# Flags
LIBS = -L.. -largparse -lvector
FLAGS = -I../include -g

# Files
SRC = $(wildcard ./*.c)
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(OBJ)
	$(CC) -o ../bin/lasm $(OBJ) $(FLAGS) $(LIBS)
	ln -s ../bin/lasm ./lasm

%.o: %.c
	$(CC) -o $@ $(FLAGS) -c $<

clean:
	rm -f $(OBJ)
