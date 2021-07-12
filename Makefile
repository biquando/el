CC = gcc

# Directories
INC_DIR = include
LIB_DIR = lib
EL_SRC_DIR = src
LASM_SRC_DIR = lasm

# Flags
LIBS = -L. -largparse -lvector -pthread
FLAGS = -I$(INC_DIR) -g

# Files
EL_SRC = $(wildcard $(EL_SRC_DIR)/*.c)
EL_OBJ = $(EL_SRC:.c=.o)
LASM_SRC = $(wildcard $(LASM_SRC_DIR)/*.c)
LASM_OBJ = $(LASM_SRC:.c=.o)

.PHONY: all libs clean

all: $(EL_OBJ) $(LASM_OBJ)
	mkdir -p bin
	$(CC) -o bin/el $(EL_OBJ) $(FLAGS) $(LIBS)
	$(CC) -o bin/lasm $(LASM_OBJ) $(FLAGS) $(LIBS)

libs:
	cd lib/argparse && make libs && make
	mv lib/*/*.a .

%.o: %.c
	$(CC) -o $@ $(FLAGS) -c $<

clean:
	rm -f $(EL_OBJ) $(LASM_OBJ) ./*.a
	rm -rf bin
	cd lib/argparse && make clean
