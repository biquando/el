CC = gcc
LEX = flex

# Flags
LIBS = -L.. -largparse -lvector
FLAGS = -I../include -g

# Files
SRC = $(wildcard ./*.c)
LEXER_C = ./llex.yy.c
LEXER_H = $(LEXER_C:.c=.h)
LEXER_O = $(LEXER_C:.c=.o)
OBJ := $(SRC:.c=.o)
OBJ := $(filter-out $(LEXER_O), $(OBJ))
OBJ := $(LEXER_O) $(OBJ)
LEX_FILE = llex.l
LASM = ../bin/lasm

.PHONY: all clean

all: $(LASM)

$(LASM): $(OBJ)
	$(CC) -o $(LASM) $(OBJ) $(FLAGS) $(LIBS)
	ln -fs $(LASM) ./lasm

%.o: %.c
	$(CC) -o $@ $(FLAGS) -c $<

$(LEXER_C): $(LEX_FILE)
	$(LEX) -o $@ --header-file=$(LEXER_H) --yylineno $(LEX_FILE)

clean:
	rm -f $(OBJ) $(LEXER_C) $(LEXER_H)
