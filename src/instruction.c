#include "cpu.h"
#include "memory.h"
#include "types.h"

void __instr_load(union reg *r, byte_t h, word_t value)
{
    if (h)
        r->half = (byte_t) (value & 0xFF);
    else
        r->full = value;
}

void __instr_store(struct memory *mem, union reg *r, byte_t h, word_t addr)
{
    if (h)
        mem->data[addr] = r->half;
    else
        write_word_to_memory(mem, addr, r->full);
}

void __instr_mod(struct cpu *proc, union reg *r, byte_t h, word_t opr)
{
    unsigned int op1 = h ? r->half : r->full;
    int signex_op1 = (char) (h ? r->half : r->full);
    union reg *r2 = __select_register(proc, (opr >> 8) & 7);
    unsigned int op2 = h ? r2->half : r2->full;
    word_t mask = h ? 0xFF : 0xFFFF;
    int shift = h ? 8 : 16;
    int result;
    switch (opr & 15) {
    case 0:
        result = op1 + op2;
        goto addition_flags;
        break;
    case 1:
        result = op1 - op2;
        goto subtraction_flags;
        break;
    case 2:
        result = op1 + 1;
        break;
    case 3:
        result = op1 - 1;
        break;
    case 4:
        result = op1 & op2;
        break;
    case 5:
        result = op1 | op2;
        break;
    case 6:
        result = op1 ^ op2;
        break;
    case 7:
        result = ~op1;
        break;
    case 8:
        result = op1 << op2;
        if (op2)
            proc->fc = op1 >> (shift - op2) & 1;
        break;
    case 9:
        result = op1 >> op2;
        if (op2)
            proc->fc = op1 >> (op2 - 1) & 1;
        break;
    case 10:
        return;
    case 11:
        result = signex_op1 >> op2;
        if (op2)
            proc->fc = signex_op1 >> (op2 - 1) & 1;
        break;
    case 12:
        result = op1 - op2;
        goto subtraction_flags;
        break;
    case 13:
        return;
    case 14:
        return;
    case 15:
        return;
    }
    goto other_flags;

addition_flags:
    proc->fc = (op1 + op2) >> shift;
    goto arithmetic_flags;

subtraction_flags:
    proc->fc = op2 > op1;
    op2 = -op2;

arithmetic_flags:
    proc->fo = ((op1 >> (shift - 1) & 1) ^ (result >> (shift - 1) & 1)) &
               ((op2 >> (shift - 1) & 1) ^ (result >> (shift - 1) & 1));

other_flags:
    proc->fz = (result & mask) == 0;
    proc->fs = (result >> (shift - 1)) & 1;
    if ((opr & 15) != 12) {
        if (h)
            r->half = result & mask;
        else
            r->full = result & mask;
    }
}

void __instr_cond(struct cpu *proc, word_t opr)
{
    byte_t cond_satisfied = 0;
    switch (opr & 15) {
    case 0:
        cond_satisfied = proc->fo;
        break;
    case 1:
        cond_satisfied = !proc->fo;
        break;
    case 2:
        cond_satisfied = proc->fc;
        break;
    case 3:
        cond_satisfied = !proc->fc;
        break;
    case 4:
        cond_satisfied = proc->fz;
        break;
    case 5:
        cond_satisfied = !proc->fz;
        break;
    case 6:
        cond_satisfied = proc->fc | proc->fz;
        break;
    case 7:
        cond_satisfied = !proc->fc & !proc->fz;
        break;
    case 8:
        cond_satisfied = proc->fs;
        break;
    case 9:
        cond_satisfied = !proc->fs;
        break;
    case 10:
        cond_satisfied = 0;
        break;
    case 11:
        cond_satisfied = 0;
        break;
    case 12:
        cond_satisfied = proc->fs ^ proc->fo;
        break;
    case 13:
        cond_satisfied = !(proc->fs ^ proc->fo);
        break;
    case 14:
        cond_satisfied = proc->fz | (proc->fs ^ proc->fo);
        break;
    case 15:
        cond_satisfied = !proc->fz & !(proc->fs ^ proc->fo);
        break;
    }
    if (!cond_satisfied)
        proc->ip.full += 3;
}
