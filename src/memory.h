#ifndef _EL_MEMORY_H
#define _EL_MEMORY_H

#include "types.h"

struct memory {
    unsigned int size;
    volatile byte_t *data;
};

struct memory *initialize_memory(unsigned int size);
void free_memory(struct memory *mem);

unsigned int load_file(struct memory *mem, const char *path);

void write_word_to_memory(struct memory *mem, word_t addr, word_t val);
word_t read_word_from_memory(const struct memory *mem, word_t addr);

#endif
