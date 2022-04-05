CC = gcc
LEX = flex

# Flags
LIBS = -L.. -largparse -lvector
FLAGS = -I../include -g

# Files
SRC = $(wildcard ./*.c)
OBJ = $(SRC:.c=.o)
LEX_FILE = lasm.l
LEXER_C = lasm.yy.c
LEXER_H = $(LEXER_C:.c=.h)
LEXER_O = $(LEXER_C:.c=.o)

.PHONY: all clean

all: $(LEXER_O) $(OBJ)
	$(CC) -o ../bin/lasm $(OBJ) $(LEXER_O) $(FLAGS) $(LIBS)
	ln -fs ../bin/lasm ./lasm

%.o: %.c
	$(CC) -o $@ $(FLAGS) -c $<

$(LEXER_C):
	$(LEX) -o $@ --header-file=$(LEXER_H) $(LEX_FILE)

clean:
	rm -f $(OBJ) $(LEXER_C) $(LEXER_H) $(LEXER_O)
