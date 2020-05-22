#pragma once

#ifndef __ASM__
struct acpi_sdt {
	union {
		char sig[4];
		u32 sig_int;
	};
	u32 len;
	u8 rev;
	u8 csum;
	char oem_id[6];
	char oem_tbl_id[8];
	u32 oem_rev;
	u32 creator_id;
	u32 creator_rev;
	u8 data[0];
} __attribute__((packed));

void acpi_init(void);
const struct acpi_sdt *acpi_sdt_lookup(const char name[static 4]);
#endif
