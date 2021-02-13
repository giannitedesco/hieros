#include <hieros/types.h>
#include <hieros/kaddr.h>
#include <hieros/printf.h>
#include <hieros/printk.h>
#include <hieros/acpi.h>
#include <hieros/cpu_mask.h>
#include <hieros/cpus.h>
#include <hieros/madt.h>

struct acpi_madt {
	struct acpi_sdt sdt;
	u32 lapic_addr;
	u32 flags;
	u8 data[0];
} __attribute__((packed));

struct madt_ent {
	u8 type;
	u8 len;
	u8 data[0];
} __attribute__((packed));

struct madt_lapic {
	struct madt_ent ent;
	u8 cpu_id;
	u8 lapic_id;
	u8 flags;
} __attribute__((packed));

enum {
	MADT_LAPIC,
	MADT_IOAPIC,
	MADT_INT_OVERRIDE,
	MADT_NMI,
	MADT_LAPIC_ADDR_OVERRIDE,
};

struct cpu_mask cpus_present;
u32 bsp_cpu_id;
u8 nr_cpus_present;

static void madt_lapic(const struct madt_lapic *ent)
{
	//printf("madt: cpu=%u lapic_id=%u flags=%u\n",
	//	ent->cpu_id, ent->lapic_id, ent->flags);

	if (ent->flags & 3) {
		u8 word, bit;

		word = ent->lapic_id >> 6;
		bit = (1UL << (ent->lapic_id & 0x3f));

		if (!(cpus_present.cpu_mask[word] & bit)) {
			nr_cpus_present++;
		}
		cpus_present.cpu_mask[word] |= bit;
	}
}

void madt_init(void)
{
	const struct acpi_madt *madt;
	const u8 *ptr, *end;

	madt = (struct acpi_madt *)acpi_sdt_lookup("APIC");
	if (NULL == madt) {
		kputs("madt: MADT missing\n");
		/* panic */
		return;
	}

	ptr = madt->data;
	end = ((u8 *)madt) + madt->sdt.len;

	while(ptr < end) {
		const struct madt_ent *ent;

		ent = (struct madt_ent *)ptr;

		switch(ent->type) {
		case MADT_LAPIC:
			madt_lapic((struct madt_lapic *)ent);
			break;
		case MADT_IOAPIC:
			//kputs("madt: i/o apic\n");
			break;
		case MADT_INT_OVERRIDE:
			//kputs("madt: interrupt source override\n");
			break;
		case MADT_NMI:
			//kputs("madt: nmi\n");
			break;
		case MADT_LAPIC_ADDR_OVERRIDE:
			//kputs("madt: lapic addr override\n");
			break;
		}

		ptr += ent->len;
	}

	printf("smp: %u cpus detected\n", nr_cpus_present);
}
