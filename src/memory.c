#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "types.h"

#define min(x, y)  ((x) < (y) ? (x) : (y))

struct memory *initialize_memory(unsigned int size)
{
    struct memory *mem;
    size = min(0x10000, size);

    mem = malloc(sizeof *mem);
    mem->size = size;
    mem->data = calloc(size, sizeof mem->data);
    return mem;
}

void free_memory(struct memory *mem)
{
    free((byte_t *) mem->data);
    free(mem);
}

unsigned int load_file(struct memory *mem, const char *path)
{
    FILE *file = fopen(path, "rb");
    unsigned int bytes_written = 0;
    unsigned int idx = 0;
    int c;
    if (!file)
        return 0;

    /* Check for <Global> tag */
    c = fgetc(file);
    if (c == 0xff) {
        c = fgetc(file);
        if (c != -1)
            idx |= c;
        c = fgetc(file);
        if (c != -1)
            idx |= ((unsigned int) c) << 8;
    }

    rewind(file);
    while (idx < mem->size) {
        c = fgetc(file);
        if (c == -1)
            break;
        mem->data[idx++] = c;
    }

    fclose(file);
    return bytes_written;
}

void write_word_to_memory(struct memory *mem, word_t addr, word_t val)
{
    if (addr > mem->size - 1)
        return;

    mem->data[addr] = val & 0xFF;
    mem->data[addr + 1] = (val >> 8) & 0xFF;
}

word_t read_word_from_memory(const struct memory *mem, word_t addr)
{
    word_t val;
    if (addr > mem->size - 1)
        return 0;

    val = mem->data[addr];
    val |= ((word_t) mem->data[addr + 1]) << 8;
    return val;
}
