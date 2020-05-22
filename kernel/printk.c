#include <hieros/types.h>
#include <hieros/printk.h>
#include <hieros/printf.h>
#include <hieros/serial.h>
#include <hieros/vga.h>

int kputs(const char *str)
{
	vga_puts(str);
	return serio_puts(str);
}

void _putchar(char chr)
{
	char str[2] = {chr, '\0'};
	kputs(str);
}
