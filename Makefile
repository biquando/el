CC = gcc

# Directories
INC_DIR = include
LIB_DIR = lib
BIN_DIR = bin
EL_SRC_DIR = src
LASM_SRC_DIR = lasm

# Flags
LIBS = -L$(LIB_DIR)/argparse -largparse -L$(LIB_DIR)/vector -lvector -pthread
FLAGS = -I$(INC_DIR) -g

# Files
EL_SRC = $(wildcard $(EL_SRC_DIR)/*.c)
EL_OBJ = $(EL_SRC:.c=.o)
LASM_SRC = $(wildcard $(LASM_SRC_DIR)/*.c)
LASM_OBJ = $(LASM_SRC:.c=.o)

.PHONY: all clean

all: $(EL_OBJ) $(LASM_OBJ)
	$(CC) -o $(BIN_DIR)/el $(EL_OBJ) $(FLAGS) $(LIBS)
	$(CC) -o $(BIN_DIR)/lasm $(LASM_OBJ) $(FLAGS) $(LIBS)

%.o: %.c
	$(CC) -o $@ $(FLAGS) -c $<

clean:
	rm -f $(EL_OBJ) $(LASM_OBJ)
