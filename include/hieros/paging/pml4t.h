#pragma once

typedef struct pml4e {
	union {
		struct {
			u64 present:1;
			u64 rw:1;
			u64 user:1;
			u64 pwt:1;
			u64 pcd:1;
			u64 accessed:1;
			u64 _ignored_6:1;
			u64 _reserved_7:1;
			u64 _ignored_8_11:4;
			u64 pdpt_addr:40;
			u64 _reserved_54:1;
			u64 _ignored_52_62:10;
			u64 xd:1;
		};
		u64 word;
	};
} pml4e_t;

struct pml4t {
	pml4e_t pml4e[PGTBL_SIZE];
};

static inline pml4e_t pml4e_kernel_rx(u64 laddr)
{
	return (pml4e_t){
		.present = true,
		.pdpt_addr = pgtbl_addr(laddr),
	};
}

static inline pml4e_t pml4e_kernel_rwx(u64 laddr)
{
	return (pml4e_t){
		.present = true,
		.rw = true,
		.pdpt_addr = pgtbl_addr(laddr),
	};
}

static inline pml4e_t pml4e_user_rx(u64 laddr)
{
	return (pml4e_t){
		.present = true,
		.user = true,
		.pdpt_addr = pgtbl_addr(laddr),
	};
}

static inline pml4e_t pml4e_user_rwx(u64 laddr)
{
	return (pml4e_t){
		.present = true,
		.user = true,
		.rw = true,
		.pdpt_addr = pgtbl_addr(laddr),
	};
}
