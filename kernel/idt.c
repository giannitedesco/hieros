#include <hieros/types.h>
#include <hieros/compiler.h>
#include <hieros/printk.h>
#include <hieros/printf.h>
#include <hieros/gdt.h>
#include <hieros/desc.h>
#include <hieros/traps.h>

#include <hieros/idt.h>

#define GATE(_vec, _type, _seg, _dpl, _addr)	\
	{					\
		.vector		= _vec,		\
		.flags.type	= _type,	\
		.flags.dpl	= _dpl,		\
		.flags.p	= 1,		\
		.segment	= _seg,		\
		.addr		= _addr,	\
	}

/* Interrupt gate */
#define INTR_GATE(_vec, _addr) \
	GATE(_vec, GATE_INTR, __KERNEL_CS, DPL0, _addr)

__attribute__((used, aligned(64)))
static struct gate_desc idt[256];

static struct desc_ptr idt_ptr = {
	.size = sizeof(idt) - 1,
	.addr = (u64)idt,
};

static const struct vector defaults[] = {
	INTR_GATE(0x0, divide_error),
	INTR_GATE(0x6, invalid_opcode_error),
	INTR_GATE(0xd, gpf),
	INTR_GATE(0xe, pagefault),
	INTR_GATE(0xff, lapic_spurious),
};

static void set_idt_vector(unsigned int vec,
				u16 seg,
				void *addr,
				struct idt_flags flags)
{
	u64 offset = (u64)addr;
	idt[vec] = (struct gate_desc){
		.offset_low = (u16)offset,
		.selector = seg,
		.flags = flags,
		.offset_mid = (u16)(offset >> 16),
		.offset_high = (u32)(offset >> 32),
	};
#if 0
	printf("vec[%d].offset_low = 0x%.4x\n", vec, idt[vec].offset_low);
	printf("vec[%d].selector = 0x%.4x\n", vec, idt[vec].selector);
	printf("vec[%d].type = 0x%x\n", vec, idt[vec].flags.type);
	printf("vec[%d].dpl = 0x%x\n", vec, idt[vec].flags.dpl);
	printf("vec[%d].p = 0x%x\n", vec, idt[vec].flags.p);
	printf("vec[%d].offset_mid = 0x%.4x\n", vec, idt[vec].offset_mid);
	printf("vec[%d].offset_high = 0x%.8x\n", vec, idt[vec].offset_high);
#endif
}

static void set_vector(const struct vector *v)
{
	set_idt_vector(v->vector,
			v->segment,
			v->addr,
			v->flags);
}

void idt_apply_vectors(unsigned int nr, const struct vector tbl[static nr])
{
	unsigned int i;

	for(i = 0; i < nr; i++) {
		const struct vector *ent = &tbl[i];
		set_vector(ent);
	}
}

void idt_init(void)
{
	unsigned int i;

	for(i = 0; i < ARRAY_SIZE(idt); i++) {
		struct vector v = (struct vector)INTR_GATE(i, spurious_intr);
		set_vector(&v);
	}

	idt_apply_vectors(ARRAY_SIZE(defaults), defaults);
	load_idt(&idt_ptr);
}
