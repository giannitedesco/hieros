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
#include <hieros/types.h>

enum lapic_delivery_mode {
	LAPIC_FIXED,
	LAPIC_LOWEST_PRIO,
	LAPIC_LOWEST_SMI,
	LAPIC_DELIVERY_MODE_3,
	LAPIC_NMI,
	LAPIC_INIT,
	LAPIC_STARTUP,
	LAPIC_DELIVERY_MODE_7,
};

enum lapic_trigger {
	LAPIC_EDGE_TRIGGER,
	LAPIC_LEVEL_TRIGGER,
};

enum lapic_level {
	LAPIC_DEASSERT,
	LAPIC_ASSERT,
};

enum lapic_status {
	LAPIC_IDLE,
	LAPIC_PENDING,
};

struct lapic_icr1 {
	union {
		struct {
			u32 vector:8;
			u32 delivery_mode:3;
			u32 dest_mode:1;
			u32 delivery_status:1;
			u32 _bit13:1;
			u32 level:1;
			u32 trigger_mode:1;
			u32 _bits16_17:2;
			u32 dest_shorthand:2;
			u32 _reserved:12;
		};
		u32 word;
	};
};

struct lapic_icr2 {
	union {
		struct {
			u32 _reserved:24;
			u32 dest:8;
		};
		u32 word;
	};
};

static inline u32 lapic_icr2(u8 dest)
{
	struct lapic_icr2 reg = {
		.dest = dest,
	};
	return reg.word;
}

static inline u32 lapic_icr1_init(void)
{
	struct lapic_icr1 reg = {
		.trigger_mode = LAPIC_EDGE_TRIGGER,
		.level = LAPIC_ASSERT,
		.delivery_mode = LAPIC_INIT,
	};
	return reg.word;
}

static inline u32 lapic_icr1_sipi(u8 vector)
{
	struct lapic_icr1 reg = {
		.trigger_mode = LAPIC_EDGE_TRIGGER,
		.level = LAPIC_ASSERT,
		.delivery_mode = LAPIC_STARTUP,
		.vector = vector,
	};
	return reg.word;
}


void lapic_init(void);
bool lapic_send_init(u8 dest_id);
bool lapic_send_sipi(u8 dest_id, u8 vector);
bool lapic_poll_ack(void);
#endif
