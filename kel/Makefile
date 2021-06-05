.PHONY: all dirs test debug clean

LASM = ../bin/lasm
EL = ../bin/el
SRC_DIR = src
BIN_DIR = bin

SRC = $(patsubst $(SRC_DIR)/%,%,$(wildcard src/*.l))
MAP = $(SRC_DIR)/map.lm
BIN = $(wildcard $(BIN_DIR)/*)

all: dirs $(SRC)

dirs:
	@mkdir -p $(BIN_DIR)/

%.l:
	@$(LASM) $(SRC_DIR)/$@ -m $(MAP) -o $(BIN_DIR)/$(patsubst %.l,%,$@)

test: all
	@$(EL) $(BIN)

debug: all
	@$(EL) -d $(BIN)

clean:
	rm -r $(BIN_DIR)/
