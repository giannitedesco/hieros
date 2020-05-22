#pragma once

#ifndef __ASM__

#include <hieros/desc.h>

struct vector {
	void *addr;
	u16 segment;
	struct idt_flags flags;
	u16 vector;
} __attribute__((packed));

void idt_apply_vectors(unsigned int nr, const struct vector tbl[static nr]);
void idt_init(void);

#endif
