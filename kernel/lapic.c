#include <hieros/kernel.h>
#include <hieros/kaddr.h>
#include <hieros/cpuid.h>
#include <hieros/ioports.h>
#include <hieros/io.h>
#include <hieros/lapic.h>
#include <hieros/msr.h>
#include <hieros/traps.h>
#include <hieros/cpu_mask.h>
#include <hieros/cpus.h>

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

	//kputs("8259a: legacy PIC initialized and disabled\n");
}

static inline u32 get_reg32(const void *ptr, u16 off)
{
	return *(u32 *)(ptr + off);
}

static void set_reg32(void *ptr, u16 off, u32 val)
{
	volatile u32 *reg = (ptr + off);
	*reg = val;
}

static inline void send_command(void *base_addr, u32 cmd1, u32 cmd2)
{
	set_reg32(base_addr, LAPIC_REG_ICR2, cmd2);
	set_reg32(base_addr, LAPIC_REG_ICR1, cmd1);
}

static inline u64 get_base_msr(void)
{
	return rdmsr(IA32_APIC_BASE_MSR);
}

static inline void *base_addr_from_msr(u64 lapic_msr)
{
	return virt_addr(lapic_msr & LAPIC_BASE_MSR_MASK);
}

static inline void *get_base_addr(void)
{
	return base_addr_from_msr(get_base_msr());
}

bool lapic_send_init(u8 dest_id)
{
	void *base_addr = get_base_addr();
	send_command(base_addr,
			lapic_icr1_init(),
			lapic_icr2(dest_id));
	return false;
}

bool lapic_send_sipi(u8 dest_id, u8 vector)
{
	void *base_addr = get_base_addr();
	send_command(base_addr,
			lapic_icr1_sipi(vector),
			lapic_icr2(dest_id));

	return false;
}

bool lapic_poll_ack(void)
{
	struct lapic_icr1 icr1;
	void *base_addr;
	unsigned int i;

	base_addr = get_base_addr();
	for(i = 0; i < 1000; i++) {
		icr1.word = get_reg32(base_addr, LAPIC_REG_ICR2);
		if (icr1.delivery_status == LAPIC_IDLE)
			return true;

		inb(0x80);
	}

	kputs("lapic ack failed\n");
	return false;
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
	u64 features, base_msr;
	void *base_addr;
	bool enabled, bsp;
	u32 reg;

	i8259a_disable();

	features = cpuid_get_features();
	if ((features & CPUID_APIC) == 0) {
		panic("lapic: CPUID_APIC missing\n");
	}

	base_msr = get_base_msr();
	base_addr = base_addr_from_msr(base_msr);
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

	bsp_cpu_id = get_reg32(base_addr, LAPIC_REG_ID);
}
