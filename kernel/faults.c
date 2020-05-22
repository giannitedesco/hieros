#include <hieros/types.h>
#include <hieros/printk.h>
#include <hieros/printf.h>
#include <hieros/asm.h>

#include <hieros/traps.h>

static void print_regs(const struct regs *regs)
{
	printf("Guru meditation: err=%u(0x%x) FLAGS=%.8lx\n",
		regs->err, regs->err, regs->rflags);
	printf("rip=%.4lx:%.16lx orig_rsp=%.4lx:%.16lx\n",
		regs->cs, regs->rip, regs->ss, regs->orig_rsp);
	printf("rax=%.16lx rbx=%.16lx rcx=%.16lx rdx=%.16lx\n",
		regs->rax, regs->rbx, regs->rcx, regs->rdx);
	printf("rsi=%.16lx rdi=%.16lx rbp=%.16lx rsp=%.16lx\n",
		regs->rsi, regs->rdi, regs->rbp, regs->rsp);
	printf(" r8=%.16lx  r9=%.16lx r10=%.16lx r11=%.16lx\n",
		regs->r8, regs->r9, regs->r10, regs->r11);
	printf("r12=%.16lx r13=%.16lx r14=%.16lx r15=%.16lx\n",
		regs->r12, regs->r13, regs->r14, regs->r15);
}

static void die(const struct regs *regs)
{
	print_regs(regs);
	hang();
}

void do_spurious_intr(struct regs *regs)
{
	die(regs);
}

void do_divide_error(struct regs *regs)
{
	kputs("divide by zero..\n");
	die(regs);
}

void do_invalid_opcode_error(struct regs *regs)
{
	kputs("invalid opcode..\n");
	die(regs);
}

void do_double_fault(struct regs *regs)
{
	kputs("Double fault..\n");
	die(regs);
}

void do_tss_fault(struct regs *regs)
{
	kputs("TSS fault..\n");
	die(regs);
}

void do_seg_fault(struct regs *regs)
{
	kputs("segmentation fault..\n");
	die(regs);
}

void do_ss_fault(struct regs *regs)
{
	kputs("stack-segment fault..\n");
	die(regs);
}

void do_gpf(struct regs *regs)
{
	kputs("General protection fault...\n");
	die(regs);
}

void do_pagefault(struct regs *regs)
{
	kputs("pagefault..\n");
	die(regs);
}

void do_alignment_check(struct regs *regs)
{
	kputs("alignment check..\n");
	die(regs);
}
