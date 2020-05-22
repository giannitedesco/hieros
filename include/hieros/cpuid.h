#pragma once

#include <hieros/types.h>
#include <hieros/const.h>

/* Feature flags */
#define CPUID_FPU_BIT		0
#define CPUID_FPU		_BITUL(CPUID_FPU_BIT)
#define CPUID_VME_BIT		1
#define CPUID_VME		_BITUL(CPUID_VME_BIT)
#define CPUID_DE_BIT		2
#define CPUID_DE		_BITUL(CPUID_DE_BIT)
#define CPUID_PSE_BIT		3
#define CPUID_PSE		_BITUL(CPUID_PSE_BIT)
#define CPUID_TSC_BIT		4
#define CPUID_TSC		_BITUL(CPUID_TSC_BIT)
#define CPUID_MSR_BIT		5
#define CPUID_MSR		_BITUL(CPUID_MSR_BIT)
#define CPUID_PAE_BIT		6
#define CPUID_PAE		_BITUL(CPUID_PAE_BIT)
#define CPUID_MCE_BIT		7
#define CPUID_MCE		_BITUL(CPUID_MCE_BIT)
#define CPUID_CX8_BIT		8
#define CPUID_CX8		_BITUL(CPUID_CX8_BIT)
#define CPUID_APIC_BIT		9
#define CPUID_APIC		_BITUL(CPUID_APIC_BIT)
/*..*/
#define CPUID_SEP_BIT		11
#define CPUID_SEP		_BITUL(CPUID_SEP_BIT)
#define CPUID_MTRR_BIT		12
#define CPUID_MTRR		_BITUL(CPUID_MTRR_BIT)
#define CPUID_PGE_BIT		13
#define CPUID_PGE		_BITUL(CPUID_PGE_BIT)
#define CPUID_MCA_BIT		14
#define CPUID_MCA		_BITUL(CPUID_MCA_BIT)
#define CPUID_CMOV_BIT		15
#define CPUID_CMOV		_BITUL(CPUID_CMOV_BIT)
#define CPUID_PAT_BIT		16
#define CPUID_PAT		_BITUL(CPUID_PAT_BIT)
#define CPUID_PSE36_BIT		17
#define CPUID_PSE36		_BITUL(CPUID_PSE36_BIT)
#define CPUID_PSN_BIT		18
#define CPUID_PSN		_BITUL(CPUID_PSN_BIT)
#define CPUID_CLFLUSH_BIT	19
#define CPUID_CLFLUSH		_BITUL(CPUID_CLFLUSH_BIT)
/*..*/
#define CPUID_DS_BIT		18
#define CPUID_DS		_BITUL(CPUID_DS_BIT)
#define CPUID_ACPI_BIT		18
#define CPUID_ACPI		_BITUL(CPUID_ACPI_BIT)
#define CPUID_MMX_BIT		18
#define CPUID_MMX		_BITUL(CPUID_MMX_BIT)
#define CPUID_FXSR_BIT		18
#define CPUID_FXSR		_BITUL(CPUID_FXSR_BIT)
#define CPUID_SSE_BIT		18
#define CPUID_SSE		_BITUL(CPUID_SSE_BIT)
#define CPUID_SSE2_BIT		18
#define CPUID_SSE2		_BITUL(CPUID_SSE2_BIT)
#define CPUID_SS_BIT		18
#define CPUID_SS		_BITUL(CPUID_SS_BIT)
#define CPUID_HT_BIT		18
#define CPUID_HT		_BITUL(CPUID_HT_BIT)
#define CPUID_TM_BIT		18
#define CPUID_TM		_BITUL(CPUID_TM_BIT)
#define CPUID_IA64_BIT		18
#define CPUID_IA64		_BITUL(CPUID_IA64_BIT)
#define CPUID_PBE_BIT		18
#define CPUID_PBE		_BITUL(CPUID_PBE_BIT)

/* Extended feature flags */
#define CPUID_PDPE1GB_BIT	26
#define CPUID_PDPE1GB		_BITUL(CPUID_PDPE1GB_BIT)

#define CPUID_LM_BIT		29
#define CPUID_LM		_BITUL(CPUID_LM_BIT)

#ifndef __ASM__
static inline void cpuid(u32 op, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
	asm volatile ("cpuid"
		: "=a" (*eax),
		  "=b" (*ebx),
		  "=c" (*ecx),
		  "=d" (*edx)
		: "0" (op));
}

static inline u64 cpuid_get_features(void)
{
	u32 eax, ebx, ecx, edx;
	cpuid(1, &eax, &ebx, &ecx, &edx);
	return ((u64)ecx << 32) | edx;
}

static inline u32 cpuid_get_ext_features(void)
{
	u32 eax, ebx, ecx, edx;
	cpuid(0x80000001, &eax, &ebx, &ecx, &edx);
	return edx;
}

void cpuid_init(void);
#endif
