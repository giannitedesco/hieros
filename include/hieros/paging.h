#pragma once

#include <hieros/const.h>

#define PML4E_PRESENT	_BITUL(0)
#define PML4E_RW	_BITUL(1)
#define PML4E_USER	_BITUL(2)
#define PML4E_PWT	_BITUL(3)
#define PML4E_PCD	_BITUL(4)
#define PML4E_ACCESSED	_BITUL(5)
#define PML4E__IGNORED	_BITUL(6)
#define PML4E_XD	_BITUL(63)

#define PML4E_ADDR_MASK	0x000ffffffffff000UL
#define PML4E_ADDR(x) ((x) & PML4E_ADDR_MASK)
#define PML4E_PTR(x) ((void *)PML4E_ADDR(x))

#define PDPTE_PRESENT	_BITUL(0)
#define PDPTE_RW	_BITUL(1)
#define PDPTE_USER	_BITUL(2)
#define PDPTE_PWT	_BITUL(3)
#define PDPTE_PCD	_BITUL(4)
#define PDPTE_ACCESSED	_BITUL(5)
#define PDPTE_DIRTY	_BITUL(6)
#define PDPTE_HUGE	_BITUL(7)
#define PDPTE_GLOBAL	_BITUL(8)
#define PDPTE_PAT	_BITUL(9)
#define PDPTE_XD	_BITUL(63)

#define PDTE_PRESENT	_BITUL(0)
#define PDTE_RW		_BITUL(1)
#define PDTE_USER	_BITUL(2)
#define PDTE_PWT	_BITUL(3)
#define PDTE_PCD	_BITUL(4)
#define PDTE_ACCESSED	_BITUL(5)
#define PDTE_DIRTY	_BITUL(6)
#define PDTE_HUGE	_BITUL(7)
#define PDTE_GLOBAL	_BITUL(8)
#define PDTE_PAT	_BITUL(12)
#define PDTE_XD		_BITUL(63)

#define PGTBL_SHIFT	9U
#define PGTBL_SIZE	(1UL << PGTBL_SHIFT)
#define PGTBL_MASK	(PGTBL_SIZE - 1)

#define PAGE4K_SHIFT	12U
#define PAGE4K_SIZE	(1UL << PAGE4K_SHIFT)
#define PAGE4K_MASK	(PAGE4K_SIZE - 1)

#define PAGE2M_SHIFT	21U
#define PAGE2M_SIZE	(1UL << PAGE2M_SHIFT)
#define PAGE2M_MASK	(PAGE2M_SIZE - 1)

#define PAGE1G_SHIFT	30U
#define PAGE1G_SIZE	(1UL << PAGE1G_SHIFT)
#define PAGE1G_MASK	(PAGE1G_SIZE - 1)

#define PML4T_SHIFT	39U
#define PML4T_SIZE	(1UL << PML4T_SHIFT)
#define PML4T_MASK	(PML4T_SIZE - 1)

#define __pml4t_index(addr)	((addr >> PML4T_SHIFT) & PGTBL_MASK)
#define __pdpt_index(addr)	((addr >> PAGE1G_SHIFT) & PGTBL_MASK)
#define __pdt_index(addr)	((addr >> PAGE2M_SHIFT) & PGTBL_MASK)
#define __pt_index(addr)	((addr >> PAGE4K_SHIFT) & PGTBL_MASK)
#define __pg_offset(addr)	(addr & PAGE4K_MASK)

#ifndef __ASM__
static inline void invlpg(u64 addr)
{
	asm volatile ("invlpg (%0)" :: "r" (addr) : "memory");
}

extern unsigned long __force_order;
static inline unsigned long cr3_get(void)
{
	unsigned long val;
	asm volatile("mov %%cr3,%0\n\t" : "=r" (val), "=m" (__force_order));
	return val;
}

static inline void cr3_set(unsigned long val)
{
	asm volatile("mov %0,%%cr3": : "r" (val), "m" (__force_order));
}


static inline void flush_tlb(void)
{
	cr3_set(cr3_get());
}
#endif
