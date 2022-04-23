.PHONY: all mkdirs run debug
LASM = ../bin/lasm
EL = ../bin/el

SRC = $(wildcard src/*.lasm)
MAP = src/map.lm
BIN = $(subst src,bin,$(subst .lasm,,$(SRC)))

all: mkdirs $(BIN)

mkdirs:
	mkdir -p bin

bin/%: src/%.lasm
	$(LASM) $(MAP) $< -o $@

run: all
	$(EL) $(BIN)

debug: all
	$(EL) --debug $(BIN)

clean:
	rm -rf bin
