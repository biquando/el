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
    char *map_name = NULL;
    FILE *fsrc;
    FILE *fout;
    FILE *fmap;
    struct buffer *unres_buf = initialize_buffer(256);
    struct buffer *res_buf = initialize_buffer(256);
    struct symtab *sym = initialize_symtab();
    char line_buffer[LINE_BUFFER_SIZE];
    char *tmp;

    /* Parse arguments */
    for (i = 1; i < argc; i++) {
        if (!strncmp("-o", argv[i], 3) || !strncmp("--out", argv[i], 6)) {
            if (i == argc - 1 || out_name) {
                goto arg_error;
            }
            out_name = argv[++i];
            continue;
        }
        if (!strncmp("-m", argv[i], 3) || !strncmp("--map", argv[i], 6)) {
            if (i == argc - 1 || map_name) {
                goto arg_error;
            }
            map_name = argv[++i];
            continue;
        }
        if (src_name) {
            goto arg_error;
        }
        src_name = argv[i];
        continue;

    arg_error:
        fprintf(stderr, "%s: error with arg %s\n", EXEC_NAME, argv[i]);
        return 2;
    }

    /* Open src file */
    fsrc = fopen(src_name, "r");
    if (!fsrc) {
        fprintf(stderr, "%s: could not open: %s\n", EXEC_NAME, src_name);
        return 2;
    }

    /* Parse map file */
    if (map_name) {
        fmap = fopen(map_name, "r");
        if (!fmap) {
            fprintf(stderr, "%s: could not open: %s\n", EXEC_NAME, map_name);
            return 2;
        }

        /* Resolve <Locations> */
        sym->mapping = 1;
        while (1) {
            tmp = fgets(line_buffer, LINE_BUFFER_SIZE, fmap);
            if (!tmp)
                break;
            /* Parse line */
            if (tmp[0] == '\0' || (tmp[0] == '/' && tmp[1] == '/')) {
                continue;
            }
            parse_label(tmp, unres_buf, sym);
        }
        sym->mapping = 0;
    }

    /* Resolve <Labels> */
    while(1) {
        tmp = fgets(line_buffer, LINE_BUFFER_SIZE, fsrc);
        if (!tmp)
            break;
        parse_line(tmp, unres_buf, sym);
    }
    sym->resolved_symbols = 1;
    free_buffer(unres_buf);
    rewind(fsrc);

    /* Fill <Locations> and <Labels> */
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
