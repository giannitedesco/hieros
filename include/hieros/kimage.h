#pragma once

#include <hieros/types.h>

/* all the linker symbols */
extern u8 _kernel_end;
extern u8 _very_end;

extern u64 __boot_pml4t;
extern u64 __boot_pdpt;
extern u64 __boot_pdt;
