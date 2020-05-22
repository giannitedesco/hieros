#include <hieros/types.h>
#include <hieros/printk.h>
#include <hieros/kaddr.h>
#include <hieros/paging.h>
#include <hieros/kimage.h>
#include <hieros/bootmem.h>

static u8 *bootmem_ptr = &_very_end;
static u8 *bootmem_end = &_very_end;

void bootmem_init(u64 bootmem_top)
{
	bootmem_end = __va(bootmem_top);
}

void *bootmem_alloc_page(unsigned int nr_pages)
{
	u8 *ret = bootmem_ptr;
	bootmem_ptr += (nr_pages << PAGE4K_SHIFT);
	if (bootmem_ptr > bootmem_end) {
		kputs("Ran out boot memory!");
		return NULL;
	}
	return ret;
}
