#include <hieros/types.h>
#include <hieros/kaddr.h>
#include <hieros/printf.h>
#include <hieros/printk.h>
#include <hieros/acpi.h>

#define RSDP_SIG 0x2052545020445352UL /* "RSDP PTR " */
struct rsdp_base {
	u64 sig;
	u8 csum;
	char oem_id[6];
	u8 rev;
	u32 rsdt_addr;
} __attribute__((packed));

struct rsdp_ext {
	struct rsdp_base base;
	u32 len;
	u64 xsdt_addr;
	u8 ext_csum;
	u8 reserved[3];
} __attribute__((packed));

static u32 rsdt_ptr;

const struct acpi_sdt *acpi_sdt_lookup(const char name[static 4])
{
	const struct acpi_sdt *r = virt_addr(rsdt_ptr);
	const u8 *ptr, *end;
	u32 sig = *(u32 *)name;
	u32 addr;

	ptr = r->data;
	end = (u8 *)r + r->len;

	for(; ptr < end; ptr += sizeof(addr)) {
		const struct acpi_sdt *ret;
		addr = *(u32 *)ptr;
		ret = virt_addr(addr);
		if (ret->sig_int == sig)
			return ret;
	}

	return NULL;
}

static bool parse_rsdt(u32 addr)
{
	const struct acpi_sdt *r = virt_addr(addr);

	printf("acpi: v%u %.4s @ 0x%lx: %.6s\n",
		r->rev, r->sig, addr, r->oem_id);

	/* TODO: validate the checksum */

	rsdt_ptr = addr;

	return true;
}

static bool parse_rsdp(u32 addr)
{
	const struct rsdp_ext *r = virt_addr(addr);

	printf("acpi: ACPI v%s RSDP @ 0x%lx: %.6s\n",
		(r->base.rev) == 0 ? "1" : "2+", addr, r->base.oem_id);

	/* TODO: validate the checksum */

	return parse_rsdt(r->base.rsdt_addr);
}

static bool locate_rsdp(u32 addr, u32 len)
{
	u32 end_addr = addr + len;

	for(; addr + sizeof(struct rsdp_base) <= end_addr; addr += 16) {
		const u8 *ptr = virt_addr(addr);
		u64 sig = *(u64 *)ptr;
		if (sig == RSDP_SIG) {
			return parse_rsdp(addr);
		}
	}

	return false;
}


#define BIOS_BDA_ADDR		0x00000400
#define BIOS_BDA_EBDA_BASE	0xe
#define EBDA_AREA_SIZE		1024

static u16 bda_get_word(u16 off)
{
	return *(u16 *)virt_addr(BIOS_BDA_ADDR + off);
}

static u32 bda_get_ebda_base_addr(void)
{
	return (u32)bda_get_word(BIOS_BDA_EBDA_BASE) << 4;
}

#define RSDP_HI_AREA		0xe0000
#define RSDP_HI_AREA_SIZE	0x20000
static bool rip_rsdp(void)
{
	if (locate_rsdp(bda_get_ebda_base_addr(), EBDA_AREA_SIZE)) {
		return true;
	}

	if (locate_rsdp(RSDP_HI_AREA, RSDP_HI_AREA_SIZE)) {
		return true;
	}

	return false;
}

void acpi_init(void)
{
	if (!rip_rsdp())
		return; // panic
}
