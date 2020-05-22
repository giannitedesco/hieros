#pragma once

#include <hieros/types.h>

#ifndef __ASM__

struct regs {
	u64 rax;
	u64 rbx;
	u64 rcx;
	u64 rdx;
	u64 rsi;
	u64 rdi;
	u64 rbp;
	u64 rsp;
	u64 r8;
	u64 r9;
	u64 r10;
	u64 r11;
	u64 r12;
	u64 r13;
	u64 r14;
	u64 r15;

	u64 err;
	u64 rip;
	u64 cs;
	u64 rflags;
	u64 orig_rsp;
	u64 ss;
} __attribute__((packed));

#endif
