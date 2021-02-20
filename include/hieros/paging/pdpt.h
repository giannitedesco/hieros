#pragma once

typedef struct pdpte {
	union {
		struct {
			u64 present:1;
			u64 rw:1;
			u64 user:1;
			u64 pwt:1;
			u64 pcd:1;
			u64 accessed:1;
			u64 dirty:1;
			u64 huge1g:1;
			u64 global:1;
			u64 _ignored_9_11:3;
			u64 pat:1;
			u64 huge1g_addr:39;
			u64 _reserved_51:1;
			u64 _ignored_52_58:6;
			u64 pkey:4;
			u64 xd:1;
		}huge;
		struct {
			u64 present:1;
			u64 rw:1;
			u64 user:1;
			u64 pwt:1;
			u64 pcd:1;
			u64 accessed:1;
			u64 dirty:1;
			u64 huge1g:1;
			u64 global:1;
			u64 _ignored_9_11:3;
			u64 pdt_addr:40;
			u64 _reserved_51:1;
			u64 _ignored_52_58:6;
			u64 pkey:4;
			u64 xd:1;
		};
		u64 word;
	};
} pdpte_t;

struct pdpt {
	pdpte_t pdpte[PGTBL_SIZE];
};

static inline u64 pdpt_huge1g_addr(u64 laddr)
{
	return (laddr & 0xffffffffc0000000) >> PDPTE_PAGE1G_SHIFT;
}

static inline pdpte_t pdpte_kernel_rx(u64 laddr)
{
	return (pdpte_t){
		.present = true,
		.pdt_addr = pgtbl_addr(laddr),
	};
}

static inline pdpte_t pdpte_kernel_rwx(u64 laddr)
{
	return (pdpte_t){
		.present = true,
		.rw = true,
		.pdt_addr = pgtbl_addr(laddr),
	};
}

static inline pdpte_t pdpte_user_rx(u64 laddr)
{
	return (pdpte_t){
		.present = true,
		.user = true,
		.pdt_addr = pgtbl_addr(laddr),
	};
}

static inline pdpte_t pdpte_user_rwx(u64 laddr)
{
	return (pdpte_t){
		.present = true,
		.user = true,
		.rw = true,
		.pdt_addr = pgtbl_addr(laddr),
	};
}

static inline pdpte_t pdpte_kernel_1g_rx(u64 laddr)
{
	return (pdpte_t){
		.huge.present = true,
		.huge.huge1g = true,
		.huge.huge1g_addr = pdpt_huge1g_addr(laddr),
	};
}

static inline pdpte_t pdpte_kernel_1g_rwx(u64 laddr)
{
	return (pdpte_t){
		.huge.present = true,
		.huge.rw = true,
		.huge.huge1g = true,
		.huge.huge1g_addr = pdpt_huge1g_addr(laddr),
	};
}

static inline pdpte_t pdpte_user_1g_rx(u64 laddr)
{
	return (pdpte_t){
		.huge.present = true,
		.huge.user = true,
		.huge.huge1g = true,
		.huge.huge1g_addr = pdpt_huge1g_addr(laddr),
	};
}

static inline pdpte_t pdpte_user_1g_rwx(u64 laddr)
{
	return (pdpte_t){
		.huge.present = true,
		.huge.rw = true,
		.huge.user = true,
		.huge.huge1g = true,
		.huge.huge1g_addr = pdpt_huge1g_addr(laddr),
	};
}
