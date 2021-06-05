#include <stdio.h>
#include <string.h>
#include "buffer.h"
#include "parse.h"

#define LINE_BUFFER_SIZE 256

int main(int argc, char *argv[])
{
    const char *DEFAULT_OUT_NAME = "a.bin";
    const char *EXEC_NAME = "lasm";
    int i;
    char *src_name = NULL;
    char *out_name = NULL;
    FILE *fsrc;
    FILE *fout;
    struct buffer *unres_buf = initialize_buffer(256);
    struct buffer *res_buf = initialize_buffer(256);
    struct symtab *sym = initialize_symtab();
    char line_buffer[LINE_BUFFER_SIZE];
    char *tmp;

    /* Parse arguments into src_name and out_name */
    switch (argc) {
    case 2:
        src_name = argv[1];
        out_name = (char *) DEFAULT_OUT_NAME;
        break;
    case 4:
        if (!strncmp(argv[1], "-o", 3)) {
            out_name = argv[2];
            src_name = argv[3];
            break;
        } else if (!strncmp(argv[2], "-o", 3)) {
            src_name = argv[1];
            out_name = argv[3];
            break;
        }
        /* Else, fall through */
    default:
        fprintf(stderr, "%s: invalid arguments\n", EXEC_NAME);
        return 1;
    }

    /* Open src file */
    fsrc = fopen(src_name, "r");
    if (!fsrc) {
        fprintf(stderr, "%s: could not open file: '%s'\n", EXEC_NAME, src_name);
        return 2;
    }

    /* Resolve labels */
    while(1) {
        tmp = fgets(line_buffer, LINE_BUFFER_SIZE, fsrc);
        if (!tmp)
            break;
        parse_line(tmp, unres_buf, sym);
    }
    sym->resolved_symbols = 1;
    free_buffer(unres_buf);
    rewind(fsrc);

    /* Fill labels */
    while (1) {
        tmp = fgets(line_buffer, LINE_BUFFER_SIZE, fsrc);
        if (!tmp)
            break;
        parse_line(tmp, res_buf, sym);
    }
    free_symtab(sym);
    fclose(fsrc);

    /* Write res_buf to fout */
    fout = fopen(out_name, "wb");
    for (i = 0; i < res_buf->size; i++) {
        fputc(res_buf->p[i], fout);
    }

    free_buffer(res_buf);
    fclose(fout);
    return 0;
}
