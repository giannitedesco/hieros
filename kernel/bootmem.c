#include <hieros/types.h>
#include <hieros/printk.h>
#include <hieros/kaddr.h>
#include <hieros/paging.h>
#include <hieros/kimage.h>
#include <hieros/bootmem.h>
#include <hieros/string.h>

static u8 *bootmem_ptr = (u8 *)_very_end;
static u8 *bootmem_end = (u8 *)_very_end;

void bootmem_init(u64 bootmem_top)
{
	bootmem_end = __va(bootmem_top);
}

void *bootmem_alloc_page(unsigned int nr_pages)
{
	u8 *ret;

	ret = bootmem_ptr;
	bootmem_ptr += (nr_pages << PAGE4K_SHIFT);

	if (bootmem_ptr > bootmem_end) {
		kputs("Ran out of boot memory!");
		return NULL;
	}

	return ret;
}

void *bootmem_zalloc_page(unsigned int nr_pages)
{
	void *ret;

	ret = bootmem_alloc_page(nr_pages);
	if (ret)
		memset(ret, 0, PAGE4K_SIZE);

	return ret;
}
