#ifndef _LASM_BUFFER_H
#define _LASM_BUFFER_H

struct buffer {
    unsigned int max_size;
    unsigned int size;
    unsigned int delta;
    unsigned char *p;
};

struct buffer *initialize_buffer(unsigned int size);
void add_byte_buffer(struct buffer *buf, unsigned char b);
void append_buffer(struct buffer *buf, unsigned char *b, unsigned int nelem);
void append_str_to_buffer(struct buffer *buf, char *str);
void reserve_buffer(struct buffer *buf, unsigned int ndelta);
void free_buffer(struct buffer *buf);

struct symtab {
    unsigned int size;
    char **labs;
    unsigned short *vals;
    int resolved_symbols;
    unsigned short global;
};

struct symtab *initialize_symtab();
void add_symtab(struct symtab *sym, char *lab, unsigned int len, 
                unsigned short val);
int get_symtab(struct symtab *sym, char *lab, unsigned int len);
void free_symtab(struct symtab *sym);

#endif
