#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "parse.h"

char *skip_whitespace(char *str)
{
    char *cp = str;
    if (!str)
        return NULL;

    while (1) {
        if (*cp == '\n' || *cp == '\0')
            break;
        if (*cp == ' ' || *cp == '\t')
            cp++;
        else
            return cp;
    }
    return NULL;
}

unsigned int symbol_len(char *str)
{
    unsigned int len = 0;
    if (!str)
        return 0;
    while (isgraph(str[len]))
        len++;
    return len;
}

char *next_symbol(char *str)
{
    if (!str)
        return NULL;
    char *tmp = str + symbol_len(str);
    return skip_whitespace(tmp);
}

int parse_addrmode(char *arg, unsigned int len)
{
    int mode = 0;
    if (!arg || len != 1)
        return 0;
    switch (arg[0]) {
    case '#':
        mode = 1;
        break;
    case '*':
        mode = 2;
        break;
    case '+':
        mode = 3;
        break;
    }
    return mode;
}

int parse_register(char *arg, unsigned int len)
{
    int reg = 0;
    if (!arg || len < 2 || len > 3)
        return -1;
    if (arg[0] == 'h')
        reg = 8;
    else if (arg[0] != 'r')
        return -1;

    switch (arg[1]) {
    case 'a':
        if (len == 2)
            return reg;
        else if (arg[2] == 'r')
            return reg | 5;
        break;
    case 'b':
        if (len == 2)
            return reg | 1;
        break;
    case 'c':
        if (len == 2)
            return reg | 2;
        break;
    case 'd':
        if (len == 2)
            return reg | 3;
        break;
    case 'x':
        if (len == 2)
            return reg | 4;
        break;
    case 's':
        if (len == 3 && arg[2] == 'p')
            return reg | 6;
        break;
    case 'i':
        if (len == 3 && arg[2] == 'p')
            return reg | 7;
        break;
    }
    return -1;
}

int parse_operation(char *arg, unsigned int len)
{
    int op = -1;
    if (!arg)
        return -1;
    if (len == 1) {
        switch (arg[0]) {
        case '+':
            op = 0;
            break;
        case '-':
            op = 1;
            break;
        case '&':
            op = 4;
            break;
        case '|':
            op = 5;
            break;
        case '^':
            op = 6;
            break;
        case '!':
            op = 7;
            break;
        case '?':
            op = 12;
            break;
        }
    } else if (len == 2) {
        if (arg[0] == '+' && arg[1] == '+')
            op = 2;
        else if (arg[0] == '-' && arg[1] == '-')
            op = 3;
        else if (arg[0] == '<' && arg[1] == '<')
            op = 8;
        else if (arg[0] == '>' && arg[1] == '>')
            op = 9;
        else if (arg[0] == '/' && arg[1] == '>')
            op = 11;
    }
    return op;
}

int parse_cond(char *arg, unsigned int len)
{
    int cond = -1;
    if (!arg)
        return -1;
    if (len == 1) {
        switch (arg[0]) {
        case 'o':
            cond = 0;
            break;
        case 'b':
        case 'c':
            cond = 2;
            break;
        case 'e':
        case 'z':
            cond = 4;
            break;
        case 'a':
            cond = 7;
            break;
        case 's':
            cond = 8;
            break;
        case 'l':
            cond = 12;
            break;
        case 'g':
            cond = 15;
            break;
        }
    } else if (len == 2 && arg[0] == 'n') {
        switch (arg[1]) {
        case 'o':
            cond = 1;
            break;
        case 'b':
        case 'c':
            cond = 3;
            break;
        case 'e':
        case 'z':
            cond = 5;
            break;
        case 'a':
            cond = 6;
            break;
        case 's':
            cond = 9;
            break;
        case 'l':
            cond = 13;
            break;
        case 'g':
            cond = 14;
            break;
        }
    }
    return cond;
}

int parse_sigid(char *arg, unsigned int len)
{
    int id = -1;
    if (!arg)
        return -1;
    if (len == 5 && !strncmp(arg, "abort", len)) {
        id = 0;
    } else if (len == 3 && !strncmp(arg, "out", len)) {
        id = 1;
    }
    return id;
}

int parse_hexbyte(char *arg)
{
    int num;
    char tmp[3];
    if (symbol_len(arg) != 2 || !isxdigit(arg[0]) || !isxdigit(arg[1]))
        return -1;
    strncpy(tmp, arg, 2);
    tmp[2] = '\0';
    num = strtol(tmp, NULL, 16);
    return num;
}

int parse_char(char *arg, unsigned int len)
{
    if (!arg || len < 3 || arg[0] != '\'' || arg[len - 1] != '\'' || len > 4 || 
            (len == 4 && arg[1] != '\\')) {
        return -1;
    }
    if (len == 4)
        return parse_escape(arg[2]);
    return (unsigned char) arg[1];
}

int parse_escape(char c)
{
    switch (c) {
    case 'a':
        return '\a';
    case 'b':
        return '\b';
    case 'e':
        return '\e';
    case 'f':
        return '\f';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    case 'v':
        return '\v';
    case '\\':
        return '\\';
    case '\'':
        return '\'';
    case '"':
        return '\"';
    case '?':
        return '\?';
    default:
        return -1;
    }
}

int parse_string(char *arg, unsigned int len, char *out)
{
    int i;
    int chars_written = 0;
    int tmp;
    if (!arg || len < 2 || arg[0] != '"' || arg[len - 1] != '"')
        return -1;

    for (i = 1; i < len - 1; i++) {
        if (arg[i] == '\\') {
            if (i == len - 2)
                return -1;
            tmp =  parse_escape(arg[++i]);
            if (tmp == -1)
                return -1;
            out[chars_written++] = tmp;
        } else {
            out[chars_written++] = arg[i];
        }
    }
    out[chars_written] = '\0';
    return chars_written;
}

int parse_quote_len(char **arg, char q)
{
    int len = 0;
    if (!arg || !(*arg) || (*arg)[0] != q)
        return -1;
    while ((*arg)[len+1] != '\0') {
        if ((*arg)[len+1] == q && (*arg)[len] != '\\') {
            *arg = *arg + len + 2;
            return len;
        }
        len++;
    }
    return -1;
}

void parse_line(char *str, struct buffer *buf, struct symtab *sym)
{
    str = skip_whitespace(str);
    if (!str)
        return;
    
    if (islower(str[0])) {
        parse_instr(str, buf, sym);
    } else if (isupper(str[0])) {
        parse_macro(str, buf, sym);
    } else if (str[0] == '$') {
        parse_label(str, buf, sym);
    } else if (str[0] == '/' && symbol_len(str) > 1 && str[1] == '/') {
        return;
    } else {
        fprintf(stderr, "Unknown token: %s\n", str);
        exit(3);
    }
}

void parse_instr(char *str, struct buffer *buf, struct symtab *sym)
{
    char *arg1 = next_symbol(str);
    char *arg2 = next_symbol(arg1);
    char *arg3 = next_symbol(arg2);
    char *arg4 = next_symbol(arg3);
    unsigned int len0 = symbol_len(str);
    unsigned int len1 = symbol_len(arg1);
    unsigned int len2 = symbol_len(arg2);
    unsigned int len3 = symbol_len(arg3);

    unsigned char opc = 0;
    unsigned short opr = 0;
    char *error_msg = "Invalid instruction or number of operands";

    /* <Load/Store> */
    int addrmode = parse_addrmode(arg1, len1); 
    int reg = parse_register(arg2, len2);
    int imm = parse_number(arg3, len3, sym, buf);

    /* <Modify> */
    int destreg = parse_register(arg1, len1);
    int op = parse_operation(arg2, len2);
    int opreg = parse_register(arg3, len3);

    /* <Conditional> */
    int condition = parse_cond(arg1, len1);

    /* <Signal> */
    int sigid = parse_sigid(arg1, len1);
    int sigimm = parse_number(arg2, len2, sym, buf);

    if (len0 == 3) {
        /* <Modify> */
        if (!strncmp(str, "mod", len0) && arg1 && arg2) {
            if (destreg == -1 || op == -1) {
                error_msg = "Invalid operand";
                goto invalid_opr;
            }
            opc = 0x80 | destreg;
            opr = op;
            if (op != 2 && op != 3 && op != 7) {
                if (opreg == -1) {
                    error_msg = "Missing/invalid register operand";
                    goto invalid_opr;
                }
                opr |= opreg << 8;
            } else if (arg3) {
                error_msg = "Was not expecting a register operand";
                goto invalid_opr;
            }
        /* <Signal> */
        } else if (!strncmp(str, "sig", len0) && arg1 &&
                   !(arg3 && arg2[0] != '*')) {
            if (sigid == -1) {
                error_msg = "Invalid signal type";
                goto invalid_opr;
            }
            opc = sigid;
            /* These signals need immediates */
            if (sigid == 0 || sigid == 1) {
                if (sigimm == -1) {
                    error_msg = "Missing/invalid signal immediate";
                    goto invalid_opr;
                }
                opr = sigimm & 0xffff;
            } else if (arg2) {
                error_msg = "Was not expecting a signal immediate";
                goto invalid_opr;
            }
        /* <No-op> */
        } else if (!strncmp(str, "nop", len0) && !arg2) {
            opc = 0xff;
            opr = parse_number(arg1, len1, sym, buf);
        } else {
            goto invalid_instr;
        }
    } else if (len0 == 4) {
        /* <Load> */
        if (!strncmp(str, "load", len0) && arg1 && arg2) {
            if (addrmode == 0 || reg == -1) {
                error_msg = "Invalid operand";
                goto invalid_opr;
            }
            opc = (addrmode << 4) | reg;
            if (addrmode == 1 && imm == -1) {
                error_msg = "Missing/invalid immediate";
                goto invalid_opr;
            }
            opr = imm & 0xffff;
        /* <Conditional> */
        } else if (!strncmp(str, "cond", len0) && arg1 && !arg2) {
            if (condition == -1) {
                error_msg = "Invalid condition";
                goto invalid_opr;
            }
            opc = 0xc0;
            opr = condition;
        } else {
            goto invalid_instr;
        }
    } else if (len0 == 5) {
        /* <Store> */
        if (!strncmp(str, "store", len0) && arg1 && arg2) {
            if (addrmode == 0 || addrmode == 1 || reg == -1) {
                error_msg = "Invalid operand";
                goto invalid_opr;
            }
            opc = 0x40 | (addrmode << 4) | reg;
            opr = imm & 0xff;
        } else {
            goto invalid_instr;
        }
    } else {
        goto invalid_instr;
    }
    if (arg4) {
        error_msg = "Too many operands";
        goto invalid_opr;
    }

    add_byte_buffer(buf, opc);
    add_byte_buffer(buf, opr & 0xff);
    add_byte_buffer(buf, (opr >> 8) & 0xff);
    return;

invalid_instr:
    fprintf(stderr, "%s: %s\n", error_msg, str);
    exit(3);
invalid_opr:
    fprintf(stderr, "%s: %s\n", error_msg, str);
    exit(4);
}

void parse_macro(char *str, struct buffer *buf, struct symtab *sym)
{
    char *arg = next_symbol(str);
    unsigned int len = symbol_len(arg);
    unsigned int len0 = symbol_len(str);
    char *error_msg = "Invalid macro or missing operands";
    int tmp;
    char *tmpcp;
    struct buffer *tmp_buf = initialize_buffer(16);
    tmp_buf->delta = 16;

    if (len0 == 4) {
        /* <Goto> */
        if (!strncmp(str, "GOTO", len0)) {
            tmp = parse_number(arg, len, sym, buf) - 3;
            if (tmp == -4 || next_symbol(arg)) {
                error_msg = "Invalid operand";
                goto error;
            }
            add_byte_buffer(tmp_buf, 0x17);
            add_byte_buffer(tmp_buf, tmp & 0xff);
            add_byte_buffer(tmp_buf, (tmp >> 8) & 0xff);
        /* <Call> */
        } else if (!strncmp(str, "CALL", len0)) {
            if (parse_number(arg, len, sym, buf) == -1 || next_symbol(arg)) {
                error_msg = "Invalid operand";
                goto error;
            }
            tmpcp = malloc(14 * sizeof *tmpcp);
            strcpy(tmpcp, "load # rx 18");
            parse_line(tmpcp, buf, sym);
            strcpy(tmpcp, "mod rx + rip");
            parse_line(tmpcp, buf, sym);
            strcpy(tmpcp, "PSH rx");
            parse_line(tmpcp, buf, sym);
            strcpy(tmpcp, "GOTO ");
            strcat(tmpcp, arg);
            parse_line(tmpcp, buf, sym);
            free(tmpcp);
        }
    } else if (len0 == 3) {
        /* <Raw> */
        if (!strncmp(str, "RAW", len0)) {
            while (arg) {
                tmp = parse_hexbyte(arg);
                if (tmp == -1) {
                    error_msg = "Invalid operand";
                    goto error;
                }
                add_byte_buffer(tmp_buf, tmp & 0xff);
                arg = next_symbol(arg);
            }
        /* <Integer> */
        } else if (!strncmp(str, "INT", len0)) {
            tmp = parse_number(arg, len, sym, buf);
            if (tmp == -1 || next_symbol(arg)) {
                error_msg = "Invalid operand";
                goto error;
            }
            add_byte_buffer(tmp_buf, tmp & 0xff);
            add_byte_buffer(tmp_buf, (tmp >> 8) & 0xff);
        /* <Character> */
        } else if (!strncmp(str, "CHA", len0)) {
            tmpcp = arg;
            tmp = parse_quote_len(&tmpcp, '\'');
            if (tmp == -1) {
                error_msg = "Invalid operand";
                goto error;
            }
            if (next_symbol(tmpcp)) {
                error_msg = "Too many operands";
                goto error;
            }
            tmp = parse_char(arg, tmp + 2);
            if (tmp == -1) {
                error_msg = "Invalid operand";
                goto error;
            }
            add_byte_buffer(tmp_buf, tmp & 0xff);
        /* <String> */
        } else if (!strncmp(str, "STR", len0)) {
            tmpcp = arg;
            tmp = parse_quote_len(&tmpcp, '"');
            if (tmp == -1) {
                error_msg = "Invalid operand";
                goto error;
            }
            if (next_symbol(tmpcp)) {
                error_msg = "Too many operands";
                goto error;
            }
            tmpcp = malloc((tmp + 1) * sizeof *tmpcp);
            tmp = parse_string(arg, tmp + 2, tmpcp);
            if (tmp == -1) {
                error_msg = "Invalid operand";
                goto error;
            }
            if (tmpcp[tmp]) {
                error_msg = "Unknown error in parsing string";
                goto error;
            }
            append_str_to_buffer(tmp_buf, tmpcp);
            free(tmpcp);
        /* <Push> */
        } else if (!strncmp(str, "PSH", len0)) {
            tmp = parse_register(arg, len);
            if (tmp == -1 || next_symbol(arg)) {
                error_msg = "Invalid operand";
                goto error;
            }
            len = (tmp & 8) ? 12 : 15;
            tmpcp = calloc(len, sizeof *tmpcp);
            len -= 9;
            tmpcp[0] = 0x86;
            tmpcp[1] = 0x03;
            if (!(tmp & 8)) {
                tmpcp[3] = 0x86;
                tmpcp[4] = 0x03;
            }
            tmpcp[len] = 0x15;
            tmpcp[len+3] = 0x85;
            tmpcp[len+4] = 0x05;
            tmpcp[len+5] = 0x06;
            tmpcp[len+6] = 0x60 | tmp;
            tmpcp[len+7] = 0xFF;
            append_buffer(tmp_buf, tmpcp, len + 9);
            free(tmpcp);
        /* <Pop> */
        } else if (!strncmp(str, "POP", len0)) {
            tmp = parse_register(arg, len);
            if (tmp == -1 || next_symbol(arg)) {
                error_msg = "Invalid operand";
                goto error;
            }
            len = (tmp & 8) ? 12 : 15;
            tmpcp = calloc(len, sizeof *tmpcp);
            len -= 3;
            tmpcp[0] = 0x15;
            tmpcp[3] = 0x85;
            tmpcp[4] = 0x05;
            tmpcp[5] = 0x06;
            tmpcp[6] = 0x86;
            tmpcp[7] = 0x02;
            if (!(tmp & 8)) {
                tmpcp[9] = 0x86;
                tmpcp[10] = 0x02;
            }
            tmpcp[len] = 0x20 | tmp;
            tmpcp[len+1] = 0xFF;
            tmpcp[len+2] = 0xFF;
            append_buffer(tmp_buf, tmpcp, len + 3);
            free(tmpcp);
        /* <Return> */
        } else if (!strncmp(str, "RET", len0) && !arg) {
            tmpcp = malloc(8 * sizeof *tmpcp);
            strncpy(tmpcp, "POP rip", 8);
            parse_line(tmpcp, buf, sym);
            free(tmpcp);
        /* <Increment> */
        } else if (!strncmp(str, "INC", len0)) {
            tmp = parse_register(arg, len);
            if (tmp == -1) {
                error_msg = "Invalid register";
                goto error;
            }
            add_byte_buffer(tmp_buf, 0x80 | tmp);
            add_byte_buffer(tmp_buf, 2);
            add_byte_buffer(tmp_buf, 0);
            add_byte_buffer(tmp_buf, 0x80 | tmp);
            add_byte_buffer(tmp_buf, 2);
            add_byte_buffer(tmp_buf, 0);
        /* <Decrement> */
        } else if (!strncmp(str, "DEC", len0)) {
            tmp = parse_register(arg, len);
            if (tmp == -1) {
                error_msg = "Invalid register";
                goto error;
            }
            add_byte_buffer(tmp_buf, 0x80 | tmp);
            add_byte_buffer(tmp_buf, 3);
            add_byte_buffer(tmp_buf, 0);
            add_byte_buffer(tmp_buf, 0x80 | tmp);
            add_byte_buffer(tmp_buf, 3);
            add_byte_buffer(tmp_buf, 0);
        } else if (!strncmp(str, "TRA", len0)) {
            tmp = parse_register(arg, len);
            if (tmp == -1) {
                error_msg = "Invalid register";
                goto error;
            }
            add_byte_buffer(tmp_buf, 0x10 | tmp);
            add_byte_buffer(tmp_buf, 0);
            add_byte_buffer(tmp_buf, 0);
            add_byte_buffer(tmp_buf, 0x80 | tmp);
            add_byte_buffer(tmp_buf, 0);
            
            arg = next_symbol(arg);
            tmp = parse_register(arg, symbol_len(arg));
            if (tmp == -1) {
                error_msg = "Invalid register";
                goto error;
            }
            add_byte_buffer(tmp_buf, tmp);
        }
    /* <Global> */
    } else if (len0 == 6 && !strncmp(str, "GLOBAL", len0) && arg) {
        if (buf->size != 0) {
            error_msg = "Global must be at the beginning of the file";
            goto error;
        }
        tmp = parse_number(arg, len, sym, buf);
        if (tmp == -1 || next_symbol(arg)) {
            error_msg = "Invalid operand";
            goto error;
        }
        sym->global = tmp & 0xffff;
        add_byte_buffer(tmp_buf, 0xff);
        add_byte_buffer(tmp_buf, tmp & 0xff);
        add_byte_buffer(tmp_buf, (tmp >> 8) & 0xff);
    } else {
        goto error;
    }

    append_buffer(buf, tmp_buf->p, tmp_buf->size);
    free_buffer(tmp_buf);
    return;

error:
    fprintf(stderr, "%s: %s\n", error_msg, str);
    exit(5);
}

void parse_label(char *str, struct buffer *buf, struct symtab *sym)
{
    unsigned int len = symbol_len(str);
    char *error_msg = "Invalid label", *tmp_str;
    int i;
    if (sym->resolved_symbols)
        goto continue_line;
    if (len < 2) {
        error_msg = "Label must not be empty";
        goto error;
    }
    for (i = 1; i < len; i++) {
        if (!isalnum(str[i]) && str[i] != '_') {
            error_msg = "Label contains invalid characters";
            goto error;
        }
    }
    if (sym->mapping) {
        tmp_str = next_symbol(str);
        i = parse_number(tmp_str, symbol_len(tmp_str), sym, buf);
        if (i == -1) {
            error_msg = "Invalid map constant";
            goto error;
        }
        add_symtab(sym, str, len, i);
    } else {
        add_symtab(sym, str, len, buf->size + sym->global);
    }

continue_line:
    if (!sym->mapping) {
        parse_line(next_symbol(str), buf, sym);
    }
    return;

error:
    fprintf(stderr, "%s: %s\n", error_msg, str);
    exit(6);
}

int parse_number(char *arg, unsigned int len, struct symtab *sym, 
                 struct buffer *buf)
{
    int num = -1;
    char *tmp;
    if (!arg || len < 1)
        return -1;
    
    if (isdigit(arg[0]) || arg[0] == '-') {
        tmp = malloc((len + 1) * sizeof *tmp);
        strncpy(tmp, arg, len);
        tmp[len] = '\0';
        num = strtol(tmp, NULL, 0) & 0xffff;
        free(tmp);
    } else if (arg[0] == '$' && len > 1) {
        num = get_symtab(sym, arg, len);
        if (num == -1 && !sym->resolved_symbols) {
            num = 0;
        }
    } else if (arg[0] == '*' && len > 2) {
        if (tmp = next_symbol(arg))
            num = parse_number(tmp, symbol_len(tmp), sym, buf);
        else
            num = 0;
        tmp = malloc((15 + len) * sizeof *tmp);
        strncpy(tmp, "load # rar ", 11);
        parse_instr(strncat(tmp, arg + 1, len), buf, sym);
        free(tmp);
    }
    return num;
}
