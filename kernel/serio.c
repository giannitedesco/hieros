#include <hieros/serial.h>
#include <hieros/ioports.h>
#include <hieros/io.h>

size_t serio_puts(const char *str)
{
	size_t ret;
	for(ret = 0; *str != '\0'; str++, ret++) {
		outb(COM1, *str);
	}
	return ret;
}
