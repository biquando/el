.PHONY: all run debug
LASM = ../bin/lasm
EL = ../bin/el
SRC_DIR = src
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.l)
MAP = $(SRC_DIR)/map.lm
BIN = $(subst $(SRC_DIR),$(BIN_DIR),$(subst .l,,$(SRC)))

all: $(BIN)

$(BIN_DIR)/%: $(SRC_DIR)/%.l
	$(LASM) $< -m $(MAP) -o $@

run: all
	$(EL) $(BIN)

debug: all
	$(EL) --debug $(BIN)

clean:
