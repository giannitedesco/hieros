#include <hieros/printk.h>
#include <hieros/cpuid.h>

static const char * const feature_names[64] = {
	"fpu",  "vme",   "de",   "pse",
	"tse",  "msr2",  "pae",  "mce",
	"cx8",  "apic",  "_r1",  "sep",
	"mtrr", "pge",   "mca",  "cmov",
	"pat",  "pse36", "psn",  "clflush",
	"_r2",  "dts",   "acpi", "mmx",
	"fxsr", "sse",   "sse2", "ss",
	"htt",  "tm",    "ia64",  "pbe",
};

static void print_features(void)
{
	u64 features;
	u8 i;
	features = cpuid_get_features();

	kputs("cpuid: ");
	for(i = 0; i < 64; i++) {
		const char *feature_name;

		if (!(features & (1U << i))) {
			continue;
		}

		feature_name = feature_names[i];
		if (!feature_name)
			continue;
		kputs(feature_name);
		kputs(" ");
	}
	kputs("\n");
}

void cpuid_init(void)
{
	print_features();
}
