CC = gcc

# Directories
INC_DIR = include
LIB_DIR = lib
BIN_DIR = bin
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
	mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/el $(EL_OBJ) $(FLAGS) $(LIBS)
	$(CC) -o $(BIN_DIR)/lasm $(LASM_OBJ) $(FLAGS) $(LIBS)

libs:
	make -C $(LIB_DIR)/argparse
	make -C $(LIB_DIR)/vector
	mv $(LIB_DIR)/*/*.a .

%.o: %.c
	$(CC) -o $@ $(FLAGS) -c $<

clean:
	-make -C $(LIB_DIR)/argparse clean
	-make -C $(LIB_DIR)/vector clean
	rm -f $(EL_OBJ) $(LASM_OBJ) ./*.a
	rm -rf $(BIN_DIR)
