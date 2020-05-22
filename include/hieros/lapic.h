#pragma once

#define LAPIC_BASE_MSR_BSP	_BITUL(8)
#define LAPIC_BASE_MSR_ENABLE	_BITUL(11)
#define LAPIC_BASE_MSR_MASK	0x1ffffff000UL

#define LAPIC_REG_ID		0x20
#define LAPIC_REG_VERS		0x30
#define LAPIC_REG_SVR		0xf0
#define LAPIC_REG_ICR1		0x300
#define LAPIC_REG_ICR2		0x310
#define LAPIC_REG_LVT3		0x370

#ifndef __ASM__
void lapic_init(void);
#endif
