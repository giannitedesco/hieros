#pragma once

#define KERNEL_MAP 0xffff800000000000UL
#define KERNEL_VMA 0xffffffff80000000UL
#define KERNEL_LMA 0x100000UL

#ifndef __ASM__

/* Translations between kernel text map and physical addr */
#define __pa(x)	((unsigned long)(x) - KERNEL_VMA)
#define __va(x)	((void *)((unsigned long)(x) + KERNEL_VMA))

/* Translations between kernel map of all physical memory and physical addr */
#define phys_addr(x) ((unsigned long)(x) - KERNEL_MAP)
#define virt_addr(x) ((void *)((unsigned long)(x) + KERNEL_MAP))

#endif
