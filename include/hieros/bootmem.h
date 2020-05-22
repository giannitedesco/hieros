#pragma once

#include <hieros/types.h>

/* Simple allocator which allocates 4KB pages at boot */
#ifndef __ASM__
void bootmem_init(u64 bootmem_top);
void *bootmem_alloc_page(unsigned int nr_pages);
#endif
