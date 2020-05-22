#pragma once

#define VGA_BASE_ADDR 0xb8000

#define VGA_COLS 80

#ifndef __ASM__
void vga_puts(const char *str);
#endif
