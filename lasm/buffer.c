#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "buffer.h"

#define DEFAULT_BUFFER_DELTA 256;

struct buffer *initialize_buffer(unsigned int size)
{
    struct buffer *buf = calloc(1, sizeof *buf);
    unsigned char *p = calloc(size, sizeof *p);
    buf->max_size = size;
    buf->size = 0;
    buf->delta = DEFAULT_BUFFER_DELTA;
    buf->p = p;
    return buf;
}

void add_byte_buffer(struct buffer *buf, unsigned char b)
{
    if (buf->size >= buf->max_size)
        reserve_buffer(buf, 1);
    buf->p[buf->size++] = b;
}

void append_buffer(struct buffer *buf, unsigned char *b, unsigned int nelem)
{
    unsigned int new_size = buf->size + nelem;
    int i;
    if (new_size > buf->max_size) {
        reserve_buffer(buf, (new_size - buf->max_size) / buf->delta + 1);
    }
    for (i = 0; i < nelem; i++) {
        buf->p[buf->size++] = b[i];
    }
}

void append_str_to_buffer(struct buffer *buf, char *str)
{
    while (*str) {
        add_byte_buffer(buf, *str);
        str++;
    }
    add_byte_buffer(buf, '\0');
}

void reserve_buffer(struct buffer *buf, unsigned int ndelta)
{
    unsigned int new_size = buf->max_size + ndelta * buf->delta;
    buf->max_size = new_size;
    buf->p = realloc(buf->p, new_size * sizeof *buf->p);
}

void free_buffer(struct buffer *buf)
{
    free(buf->p);
    free(buf);
}

struct symtab *initialize_symtab()
{
    struct symtab *sym = calloc(1, sizeof *sym);
    sym->size = 0;
    sym->labs = calloc(0, sizeof *sym->labs);
    sym->vals = calloc(0, sizeof *sym->vals);
    sym->resolved_symbols = 0;
    sym->global = 0;
    return sym;
}

void add_symtab(struct symtab *sym, char *lab, unsigned int len, 
                unsigned short val)
{
    char *labptr;
    if (len <= 1)
        return;

    labptr = malloc((len + 1) * sizeof *labptr);
    strncpy(labptr, lab, len);
    labptr[len] = '\0';
    
    sym->labs = realloc(sym->labs, (sym->size + 1) * sizeof *sym->labs);
    sym->vals = realloc(sym->vals, (sym->size + 1) * sizeof *sym->vals);
    sym->labs[sym->size] = labptr;
    sym->vals[sym->size] = val;
    sym->size++;
}

int get_symtab(struct symtab *sym, char *lab, unsigned int len)
{
    int i;

    for (i = 0; i < sym->size; i++) {
        if (len == strlen(sym->labs[i]) && !strncmp(lab, sym->labs[i], len))
            return sym->vals[i];
    }
    return -1;
}

void free_symtab(struct symtab *sym)
{
    int i;
    for (i = 0; i < sym->size; i++) {
        free(sym->labs[i]);
    }
    free(sym->labs);
    free(sym->vals);
}
