#pragma once

#define DPL0 0
#define DPL3 3

#ifndef __ASM__

struct idt_flags {
	u16 istk:3;
	u16 zero:5;
	u16 type:5;
	u16 dpl:2;
	u16 p:1;
} __attribute__((packed));

struct gate_desc {
	u16 offset_low;
	u16 selector;
	struct idt_flags flags;
	u16 offset_mid;
	u32 offset_high;
	u32 rsvd;
} __attribute__((packed));

enum {
	GATE_TASK = 0x5,
	GATE_CALL = 0xc,
	GATE_INTR = 0xe,
	GATE_TRAP = 0xf,
};

struct desc_ptr {
	u16 size;
	u64 addr;
} __attribute__((packed));

static inline void load_idt(const struct desc_ptr *idtr)
{
	asm volatile("lidt %0": :"m" (*idtr));
}

static inline void load_gdt(const struct desc_ptr *idtr)
{
	asm volatile("lgdt %0": :"m" (*idtr));
}

static inline void get_idt(const struct desc_ptr *idtr)
{
	asm volatile("sidt %0": :"m" (*idtr));
}

static inline void get_gdt(const struct desc_ptr *idtr)
{
	asm volatile("sgdt %0": :"m" (*idtr));
}

#endif
