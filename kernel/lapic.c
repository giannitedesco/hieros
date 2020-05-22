#include <hieros/types.h>
#include <hieros/kaddr.h>
#include <hieros/printf.h>
#include <hieros/printk.h>
#include <hieros/cpuid.h>
#include <hieros/ioports.h>
#include <hieros/io.h>
#include <hieros/lapic.h>
#include <hieros/msr.h>
#include <hieros/acpi.h>

#include <hieros/traps.h>

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

static u64 cpu_present[4];
static u8 nr_cpus;

static void madt_lapic(const struct madt_lapic *ent)
{
	//printf("madt: cpu=%u lapic_id=%u flags=%u\n",
	//	ent->cpu_id, ent->lapic_id, ent->flags);

	if (ent->flags & 3) {
		u8 word, bit;

		word = ent->lapic_id >> 6;
		bit = (1UL << (ent->lapic_id & 0x3f));

		if (!(cpu_present[word] & bit)) {
			nr_cpus++;
		}
		cpu_present[word] |= bit;
	}
}

static bool parse_madt(void)
{
	const struct acpi_madt *madt;
	const u8 *ptr, *end;

	madt = (struct acpi_madt *)acpi_sdt_lookup("APIC");
	if (NULL == madt)
		return false;

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

	printf("smp: %u cpus detected\n", nr_cpus);
	return true;
}

#define ICW1 0x11
#define ICW4 0x01
static void i8259a_disable(void)
{
	/* Remap master to vector 0x20 - 0x2 */
	outb(i8259a_MASTER_CMD, ICW1);
	outb(i8259a_MASTER_DATA, 0x20);
	outb(i8259a_MASTER_DATA, (1U << 2));

	/* mask all */
	outb(i8259a_MASTER_CMD, ICW4);
	outb(i8259a_MASTER_DATA, 0xff);

	/* Remap slave to vector 0x28 - 0x30 */
	outb(i8259a_MASTER_CMD, ICW1);
	outb(i8259a_MASTER_DATA, 0x28);
	outb(i8259a_MASTER_DATA, 2);

	/* mask all */
	outb(i8259a_MASTER_CMD, ICW4);
	outb(i8259a_MASTER_DATA, 0xff);

	kputs("8259a: legacy PIC initialized and disabled\n");
}

static inline u32 get_reg32(u64 base_addr, u16 off)
{
	u64 addr = KERNEL_MAP + base_addr + off;
	return *(u32 *)addr;
}

static void set_reg32(u64 base_addr, u16 off, u32 val)
{
	u64 addr = KERNEL_MAP + base_addr + off;
	volatile u64 *ptr = (void *)addr;
	*ptr = val;
}

static inline void send_command(u64 base_addr, u32 cmd1, u32 cmd2)
{
	set_reg32(base_addr, LAPIC_REG_ICR2, cmd2);
	set_reg32(base_addr, LAPIC_REG_ICR1, cmd1);

}

traplinkage void do_lapic_spurious(struct regs *regs)
{
	kputs("Spurious lapic interrupt\n");
}

traplinkage void do_lapic_error(struct regs *regs)
{
	kputs("lapic error interrupt\n");
}

void lapic_init(void)
{
	u64 features, base_msr, base_addr;
	bool enabled, bsp;
	u32 bsp_id, reg;
	unsigned int i;

	i8259a_disable();

	parse_madt();

	features = cpuid_get_features();
	if ((features & CPUID_APIC) == 0) {
		kputs("lapic: CPUID_APIC missing\n");
		/* panic */
		return;
	}

	base_msr = rdmsr(IA32_APIC_BASE_MSR);
	base_addr = (base_msr & LAPIC_BASE_MSR_MASK);
	enabled = (base_msr & LAPIC_BASE_MSR_ENABLE);
	bsp = (base_msr & LAPIC_BASE_MSR_BSP);

	/* Enable the LAPIC and set spurious vector to 0xff */
	reg = get_reg32(base_addr, LAPIC_REG_SVR);
	reg &= 0xffffff00;
	set_reg32(base_addr, LAPIC_REG_SVR, reg | 0x1ff);

	/* Set the error vector to 0xfe */
	reg = get_reg32(base_addr, LAPIC_REG_LVT3);
	reg &= 0xffffff00;
	set_reg32(base_addr, LAPIC_REG_LVT3, reg | 0xfe);

	printf("lapic: addr=0x%.16lx, enabled=%u, bsp=%u\n",
		base_addr, enabled, bsp);

	bsp_id = get_reg32(base_addr, LAPIC_REG_ID);

	for(i = 0; i <= 8 * sizeof(cpu_present); i++) {
		u8 word, bit;

		word = i >> 6;
		bit = (1UL << (i & 0x3f));

		if (i == bsp_id)
			continue;
		if (!(cpu_present[word] & bit))
			continue;

		printf("smp: TODO: SIPI lapic %u\n", i);
	}
}
