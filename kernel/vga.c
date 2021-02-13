#include <hieros/vga.h>
#include <hieros/kaddr.h>
#include <hieros/ioports.h>
#include <hieros/io.h>
#include <hieros/vga.h>

#define ATTR 0x0f

extern volatile u8 *__vga_vidptr;

#define VGA_VBASE ((u8 *)__va(VGA_BASE_ADDR))

static void next_line(void)
{
	u64 off = (u64)(__vga_vidptr - VGA_VBASE);
	off += (VGA_COLS - 1) * 2;
	off -= (off % (VGA_COLS * 2));
	__vga_vidptr = (void *)(VGA_VBASE + off);
}

static void update_cursor(void)
{
	u64 off = (u64)(__vga_vidptr - VGA_VBASE) >> 1;

	outb(CRTC_INDEX, 0xf);
	outb(CRTC_DATA, off & 0xff);
	outb(CRTC_INDEX, 0xe);
	outb(CRTC_DATA, off >> 8);
}

void vga_puts(const char *str)
{
	for(;;) {
		switch(*str) {
		case '\0':
			goto finish;
		case '\n':
			next_line();
			break;
		default:
			__vga_vidptr[0] = *str;
			__vga_vidptr[1] = ATTR;
			__vga_vidptr += 2;
		}
		str++;
	}

finish:
	update_cursor();
}
