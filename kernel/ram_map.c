#include <hieros/types.h>
#include <hieros/kaddr.h>
#include <hieros/kimage.h>
#include <hieros/printk.h>
#include <hieros/paging.h>
#include <hieros/printf.h>
#include <hieros/cpuid.h>
#include <hieros/multiboot.h>
#include <hieros/desc.h>
#include <hieros/page_alloc.h>
#include <hieros/bootmem.h>

unsigned long __force_order;

static void print_e820(const u8 *ptr, u32 len)
{
	const struct memory_map *ent;
	const u8 *end = ptr + len;

	kputs("Got e820 memory map:\n");
	while(ptr + sizeof(*ent) <= end) {
		ent = (struct memory_map *)ptr;

		printf(" - 0x%.12lx -> 0x%.12lx (%u) %s",
			ent->base_addr,
			ent->base_addr + ent->length,
			ent->type,
			(ent->type == 1) ? "RAM" : "Reserved");
		printf("\n");

		ptr += sizeof(ent->size) + ent->size;
	}
}

static u64 find_max_phys_addr(const u8 *ptr, u32 len)
{
	const struct memory_map *ent;
	const u8 *end = ptr + len;
	u64 mem_hi, seg_end;

	mem_hi = 0;

	while(ptr + sizeof(*ent) <= end) {
		ent = (struct memory_map *)ptr;

		seg_end = ent->base_addr + ent->length;
		if (seg_end > mem_hi)
			mem_hi = seg_end;

		ptr += sizeof(ent->size) + ent->size;
	}

	return mem_hi;
}

static u64 find_max_bootmem_addr(const u8 *ptr, u32 len)
{
	const struct memory_map *ent;
	const u8 *end = ptr + len;
	u64 bootmem_addr, seg_end;

	bootmem_addr = (u64)(__pa(&_very_end));

	while(ptr + sizeof(*ent) <= end) {
		ent = (struct memory_map *)ptr;

		seg_end = ent->base_addr + ent->length;
		if (bootmem_addr >= ent->base_addr
				&& bootmem_addr <= seg_end)
			return seg_end;

		ptr += sizeof(ent->size) + ent->size;
	}

	return 0;
}

static void fixup_boot_page_tables(bool use_1gb_pages)
{
	u64 *lvl4 = &__boot_pml4t;
	u64 *lvl3 = &__boot_pdpt;

	/* Remove the identity map */
	lvl4[0] = 0;

	/* Convert text map to full gigabyte */
	if (use_1gb_pages) {
		lvl3[510] = PDPTE_PRESENT | PDPTE_RW | PDPTE_HUGE;
		/* In this case __boot_pdt can be free'd */
	}
}


static bool map_ram_1g(u64 map_pages)
{
	u64 l4idx, l3idx, phys, start_addr;
	u64 *pml4t = (u64 *)&__boot_pml4t;
	unsigned int i;
	u64 *pdpt;

	start_addr = KERNEL_MAP;

	pdpt = bootmem_alloc_page(1);
	if (NULL == pdpt) {
		/* panic */
		return false;
	}

	l4idx = __pml4t_index(start_addr);
	pml4t[l4idx] = ((u64)__pa(pdpt)) | PML4E_RW | PML4E_PRESENT;

	for(phys = i = 0; i < map_pages;
			i++,
			start_addr += PAGE1G_SIZE,
			phys += PAGE1G_SIZE) {
		l3idx = __pdpt_index(start_addr);
		pdpt[l3idx] = phys | PDPTE_HUGE | PDPTE_RW | PDPTE_PRESENT;
	}

	return true;
}

static bool map_all_ram(u64 mem_hi, bool use_1gb_pages)
{
	u64 map_pages, map_pdpt;

	if (use_1gb_pages) {
		map_pages = (mem_hi + PAGE1G_MASK) >> PAGE1G_SHIFT;
	}else{
		map_pages = (mem_hi + PAGE2M_MASK) >> PAGE2M_SHIFT;
	}

	map_pdpt = (mem_hi + PML4T_MASK) >> PML4T_SHIFT;

	printf("mem: %lu MiB physical mem, "
		"will map %u x %s pages with %u pdpt\n",
		mem_hi >> 20,
		map_pages,
		use_1gb_pages ? "1G" : "2M",
		map_pdpt);

	if (use_1gb_pages) {
		return map_ram_1g(map_pages);
	}else{
		kputs("TODO: 2M page support");
		return false;
	}
}

void memory_map_init(void)
{
	struct multiboot_info *mbi = __multiboot_info;
	const u8 *mem_map = __va(mbi->mmap);
	bool huge_pages = (cpuid_get_ext_features() & CPUID_PDPE1GB);
	u64 mem_hi, bootmem_hi;
	struct desc_ptr gdt;

	if (!(mbi->flags & MBF_MMAP)) {
		kputs("memory map not present\n");
		/* panic */
		return;
	}

	print_e820(__va(mbi->mmap), mbi->mmap_length);

	bootmem_hi = find_max_bootmem_addr(mem_map, mbi->mmap_length);
	bootmem_init(bootmem_hi);

	mem_hi = find_max_phys_addr(mem_map, mbi->mmap_length);
	map_all_ram(mem_hi, huge_pages);

	fixup_boot_page_tables(huge_pages);

	/* GDT direct mapping will be gone, so better reload it here first */
	get_gdt(&gdt);
	gdt.addr = (u64)__va(gdt.addr);
	load_gdt(&gdt);

	flush_tlb();
}
