#pragma once

#include <hieros/types.h>
#include <hieros/const.h>

#define MSR_EFER		0xc0000080 /* extended features */

#define IA32_APIC_BASE_MSR	0x1B /* APIC base addr */


#define EFER_LME_BIT	8
#define EFER_LME	_BITUL(EFER_LME_BIT)

#ifndef __ASM__
static inline u64 rdmsr(u64 msr)
{
	u32 low, high;
	asm volatile (
		"rdmsr"
		: "=a"(low), "=d"(high)
		: "c"(msr)
	);
	return ((u64)high << 32) | low;
}
static inline void wrmsr(u64 msr, u64 value)
{
	u32 low = value & 0xFFFFFFFF;
	u32 high = value >> 32;
	asm volatile (
		"wrmsr"
		:
		: "c"(msr), "a"(low), "d"(high)
	);
}
#endif
