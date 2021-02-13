#pragma once

/* -2GB kernel-vma: map of the kernel.
 * Corresponds to the first 2GB of physical memory
 *
 * The significance of this mapping is that this is where symbols in the kernel
 * ELF image are located.
 */
#define KERNEL_VMA 0xffffffff80000000UL

/* -128GB RAM-map: map of installed physical memory
 * Corresponds to the first 64TB of physical memory
 */
#define RAM_MAP 0xffff800000000000UL

/* Kernel logical address
 * Offset in to physical memory where kernel image is loaded
 */
#define KERNEL_LMA 0x100000UL

#ifndef __ASM__

/* Translations between kernel text map and physical addr */
#define __pa(x)	((unsigned long)(x) - KERNEL_VMA)
#define __va(x)	((void *)((unsigned long)(x) + KERNEL_VMA))

/* Translations between kernel map of all physical memory and physical addr */
#define phys_addr(x) ((unsigned long)(x) - RAM_MAP)
#define virt_addr(x) ((void *)((unsigned long)(x) + RAM_MAP))

#endif
