#pragma once

#include <hieros/compiler.h>
#include <hieros/types.h>
#include <hieros/string.h>
#include <hieros/printk.h>
#include <hieros/printf.h>
#include <hieros/stdarg.h>

#ifndef __ASM__
static __noreturn __always_inline void ud2(void)
{
	asm volatile("ud2\n");
	__builtin_unreachable();
}

__noreturn static inline void panic(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	vprintf(fmt, va);
	ud2();
	__builtin_unreachable();
}
#endif
