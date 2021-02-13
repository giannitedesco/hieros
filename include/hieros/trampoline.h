#pragma once

#define TRAMPOLINE_ADDR 0x10000

#define TRAMPOLINE_SYM trampoline
#define TRAMPOLINE_END_SYM trampoline_end
#define NORELOC(_sym) (_sym - TRAMPOLINE_SYM)
#define NORELOC_ABS(_sym) (TRAMPOLINE_ADDR + NORELOC(_sym))

#ifndef __ASM__
extern const char TRAMPOLINE_SYM[];
extern const char TRAMPOLINE_END_SYM[];
#define TRAMPOLINE_SIZE (TRAMPOLINE_END_SYM - TRAMPOLINE_SYM)
#endif
