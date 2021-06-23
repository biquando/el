#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <argparse/argparse.h>
#include "buffer.h"
#include "parse.h"

#define LINE_BUFFER_SIZE 256
#define LASM "lasm"

int parse_error(char *msg)
{
    fprintf(stderr, LASM": %s\n", msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    const char *DEFAULT_OUT_NAME = "l.out";
    const char *EXEC_NAME = LASM;
    int i;
    struct argparse *ap = argp_init();
    char *src_name = NULL;
    char *out_name = NULL;
    char *map_name = NULL;
    char **arg_vals = malloc(argc * sizeof *arg_vals);
    FILE *fsrc;
    FILE *fout;
    FILE *fmap;
    struct buffer *unres_buf = initialize_buffer(256);
    struct buffer *res_buf = initialize_buffer(256);
    struct symtab *sym = initialize_symtab();
    char line_buffer[LINE_BUFFER_SIZE];
    char *tmp;

    /* Parse arguments */
    argp_opt(ap, "-o", 1);
    argp_alias(ap, "--out", "-o");
    argp_opt(ap, "-m", 1);
    argp_alias(ap, "--map", "-m");
    !argp_parse(ap, argc, argv) && parse_error("parse error");
    i = argp_get_opt(ap, "-o", arg_vals);
    out_name = i ? arg_vals[0] : (char*) DEFAULT_OUT_NAME;
    i = argp_get_opt(ap, "-m", arg_vals);
    map_name = i ? arg_vals[0] : NULL;
    (argp_get_args(ap, arg_vals) != 1) && parse_error("error with src .l file");
    src_name = arg_vals[0];
    argp_free(ap);
    free(arg_vals);

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

        /* Resolve <Constants> */
        sym->mapping = 1;
        while (1) {
            tmp = fgets(line_buffer, LINE_BUFFER_SIZE, fmap);
            if (!tmp)
                break;
            i = strnlen(tmp, LINE_BUFFER_SIZE);
            if (i < 1) {
                continue;
            }
            if (tmp[i - 1] == '\n') {
                tmp[i - 1] = '\0';
            }
            tmp = skip_whitespace(tmp);
            if (!tmp) {
                continue;
            }
            /* Parse line */
            if (tmp[0] == '\0' || (tmp[0] == '/' && tmp[1] == '/')) {
                continue;
            }
            parse_label(tmp, unres_buf, sym);
        }
        sym->mapping = 0;
        sym->mapped = 1;
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

    /* Fill <Constants> and <Labels> */
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
