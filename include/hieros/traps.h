#pragma once

#include <hieros/regs.h>

#ifndef __ASM__
#define traplinkage __attribute__((visibility("default")))

void spurious_intr(void);

void divide_error(void);
void invalid_opcode_error(void);

void double_fault(void);
void tss_fault(void);
void seg_fault(void);
void ss_fault(void);
void gpf(void);
void pagefault(void);
void alignment_check(void);

void lapic_spurious(void);
void lapic_error(void);

traplinkage void do_spurious_intr(struct regs *regs);

traplinkage void do_divide_error(struct regs *regs);
traplinkage void do_invalid_opcode_error(struct regs *regs);

traplinkage void do_double_fault(struct regs *regs);
traplinkage void do_tss_fault(struct regs *regs);
traplinkage void do_seg_fault(struct regs *regs);
traplinkage void do_ss_fault(struct regs *regs);
traplinkage void do_gpf(struct regs *regs);
traplinkage void do_pagefault(struct regs *regs);
traplinkage void do_alignment_check(struct regs *regs);

traplinkage void do_lapic_spurious(struct regs *regs);
traplinkage void do_lapic_error(struct regs *regs);

#endif
