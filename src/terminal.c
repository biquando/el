#include <pthread.h>
#include <stdio.h>
#include "terminal.h"
#include "interrupt.h"
#include "types.h"

void terminal_output(byte_t *start, word_t mode) {
    word_t tmp = start[0] | ((word_t) start[1] << 8);
    switch (mode) {
    case 1:
        printf("%u\n", start[0]);
        break;
    case 2:
        printf("%u\n", tmp);
        break;
    case 3:
        printf("%d\n", (char) start[0]);
        break;
    case 4:
        printf("%d\n", (short) tmp);
        break;
    case 5:
        printf("0x%X\n", tmp);
        break;
    default:
        printf("%s", start);
        break;
    }
}

void *terminal_input(void *pkg_ptr) {
    struct interrupt_pkg pkg = *((struct interrupt_pkg *) pkg_ptr);
    int i;
    volatile byte_t *start = pkg.proc->mem->data + pkg.start_addr; 

    fgets((byte_t *) start, pkg.len, stdin);
    for (i = 0; i < pkg.len; i++) {
        if (start[i] == '\n' || start[i] == '\0') {
            start[i] = '\0';
            break;
        }
    }

    (*pkg.callback)(pkg.proc, pkg.sig);
    *pkg.flag = 0;
    pthread_exit(NULL);
}
