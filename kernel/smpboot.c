#include <hieros/printk.h>
#include <hieros/printf.h>
#include <hieros/kaddr.h>
#include <hieros/smp.h>
#include <hieros/cpu_mask.h>
#include <hieros/cpus.h>
#include <hieros/trampoline.h>
#include <hieros/lapic.h>

void __ap_boot(void)
{
	kputs("AP booted\n");
}


void smp_init(void)
{
	unsigned int i;

	__builtin_memcpy(virt_addr(TRAMPOLINE_ADDR),
			trampoline,
			TRAMPOLINE_SIZE);

	for(i = 0; i <= MAX_CPUS; i++) {
		u8 word, bit;

		word = i >> 6;
		bit = (1UL << (i & 0x3f));

		if (i == bsp_cpu_id)
			continue;

		if (!(cpus_present.cpu_mask[word] & bit))
			continue;

		printf("smp: TODO: SIPI lapic %u\n", i);

		lapic_send_init(i);
		if (lapic_poll_ack()) {
			kputs("INIT ack\n");
		}

		lapic_send_sipi(i, TRAMPOLINE_ADDR >> 12);
		if (lapic_poll_ack()) {
			kputs("SIPI ack\n");
		}
	}
}
