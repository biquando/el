CC = gcc
CFLAGS = -g
EL_LDFLAGS = -pthread -g
LASM_LDFLAGS = -g

EL_SRC = $(wildcard src/*.c)
EL_OBJ = $(EL_SRC:.c=.o)
LASM_SRC = $(wildcard lasm/*.c)
LASM_OBJ = $(LASM_SRC:.c=.o)
BIN = bin
L_SRC = $(patsubst %res/src/,%,$(wildcard res/src/*.l))
L_BIN = $(wildcard res/bin/*)

.PHONY: all test_el el run_el lasm clean_el clean_lasm clean

all: dirs el lasm

test_el: dirs el run_el clean_el

dirs:
	mkdir -p $(BIN)

el: $(EL_OBJ)
	$(CC) -o $(BIN)/el $^ $(EL_LDFLAGS)

run_el: $(L_BIN)
	$(BIN)/el $^

lasm: $(LASM_OBJ)
	$(CC) -o $(BIN)/lasm $^ $(LASM_LDFLAGS)

%.l:
	$(BIN)/lasm res/src/$@ -o res/bin/$(patsubst %.l,%,$@)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean_el:
	rm $(EL_OBJ)

clean_lasm:
	rm $(LASM_OBJ)

clean: clean_el clean_lasm
	rm -r $(BIN)
