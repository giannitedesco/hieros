#include <hieros/kernel.h>
#include <hieros/cpuid.h>
#include <hieros/page_alloc.h>
#include <hieros/lapic.h>
#include <hieros/acpi.h>
#include <hieros/madt.h>
#include <hieros/idt.h>
#include <hieros/smp.h>

void kernel_main(void);
void kernel_main(void)
{
	kputs("64-bit enlightenment attained.\n");
	idt_init();
	cpuid_init();
	memory_map_init();
	acpi_init();
	madt_init();
	lapic_init();
	smp_init();
}
