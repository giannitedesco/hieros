#pragma once

#include <hieros/types.h>

/* 4 quads gives us space for 256 CPUs */
#define __CPU_MASK_QUADS 4
struct cpu_mask {
	u64 cpu_mask[__CPU_MASK_QUADS];
};

#define MAX_CPUS (BITS_PER_BYTE * sizeof(((struct cpu_mask *)0)->cpu_mask))
