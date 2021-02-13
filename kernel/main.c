#include <hieros/types.h>
#include <hieros/printk.h>
#include <hieros/cpuid.h>
#include <hieros/page_alloc.h>
#include <hieros/lapic.h>
#include <hieros/acpi.h>
#include <hieros/madt.h>
#include <hieros/idt.h>
#include <hieros/smp.h>

#include <hieros/printf.h>

void kernel_main(void);
void kernel_main(void)
{
	kputs("64-bit enlightenment attained.\n");
	cpuid_init();
	memory_map_init();
	idt_init();
	acpi_init();
	madt_init();
	lapic_init();
	smp_init();
}
