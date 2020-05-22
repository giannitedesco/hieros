#pragma once

#include <hieros/types.h>

#define MULTIBOOT_HEADER_FLAGS		0x00000003

#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

#define MBF_MEM		0x01
#define MBF_BOOTDEV	0x02
#define MBF_CMDLINE	0x04
#define MBF_MODULES	0x08
#define MBF_AOUT	0x10
#define MBF_ELF		0x20
#define MBF_MMAP	0x40

#ifndef __ASM__

/* The Multiboot header.  */
struct multiboot_header {
	u32 magic;
	u32 flags;
	u32 checksum;
	u32 header_addr;
	u32 load_addr;
	u32 load_end_addr;
	u32 bss_end_addr;
	u32 entry_addr;
};

/* The symbol table for a.out.  */
struct aout_symbol_table {
	u32 tabsize;
	u32 strsize;
	u32 addr;
	u32 reserved;
};

/* The section header table for ELF.  */
struct elf_section_header_table {
	u32 num;
	u32 size;
	u32 addr;
	u32 shndx;
};

/* The module structure.  */
struct module {
	u32 mod_start;
	u32 mod_end;
	u32 string;
	u32 reserved;
};

/* The memory map. Be careful that the offset 0 is base_addr_low but no size.
 */
struct memory_map {
	u32 size; /* size of this object */
	u64 base_addr;
	u64 length;
	u32 type;
} __attribute__((packed));

/* The Multiboot information.  */
struct multiboot_info {
	u32 flags;
	u32 mem_lower;
	u32 mem_upper;
	u32 boot_device;

	/* Command line  */
	u32 cmdline;

	/* Modules loaded by the bootloader */
	u32 mods_count;
	u32 mods_addr; /* struct module */

	/* Info about the kernel binary */
	union {
		struct aout_symbol_table aout_sym;
		struct elf_section_header_table elf_sec;
	} u;

	/* Memory maps */
	u32 mmap_length;
	u32 mmap; /* struct memory_map */
};

extern struct multiboot_info *__multiboot_info;

#endif /* ! __ASM__ */
