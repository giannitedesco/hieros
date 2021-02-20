#pragma once

#include <hieros/types.h>
#include <hieros/paging.h>

/* all the linker symbols */
extern const u8 _kernel_end[];
extern const u8 _very_end[];

extern struct pml4t __boot_pml4t;
extern struct pdpt __boot_pdpt;
extern u64 __boot_pdt[];
