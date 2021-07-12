.PHONY: all mkdirs run debug
LASM = ../bin/lasm
EL = ../bin/el

SRC = $(wildcard src/*.l)
MAP = src/map.lm
BIN = $(subst src,bin,$(subst .l,,$(SRC)))

all: mkdirs $(BIN)

mkdirs:
	mkdir -p bin

bin/%: src/%.l
	$(LASM) $< -m $(MAP) -o $@

run: all
	$(EL) $(BIN)

debug: all
	$(EL) --debug $(BIN)

clean:
	rm -rf bin
