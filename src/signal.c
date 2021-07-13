#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "signal.h"
#include "cpu.h"
#include "types.h"
#include "terminal.h"

void sig_abort(struct cpu *proc, word_t status)
{
    char id[20];
    id[0] = '\0';
    switch (status) {
    case 0:
        strcpy(id, "access error");
        break;
    case 1:
        strcpy(id, "ok");
        fprintf(stderr, "Exited with status 0x%x (%s)\n", status, id);
        exit(0);
        break;
    }

    fprintf(stderr, "Aborted with status 0x%x", status);
    if (*id)
        fprintf(stderr, " (%s)", id);
    fprintf(stderr, "\n");
    exit(!status ? 1 : status);
}

void sig_out(struct cpu *proc, word_t mode)
{
    terminal_output((byte_t *) &proc->mem->data[proc->ar.full], mode);
}
