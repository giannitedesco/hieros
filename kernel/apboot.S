#include <hieros/multiboot.h>
#include <hieros/kaddr.h>
#include <hieros/ioports.h>
#include <hieros/eflags.h>
#include <hieros/cpuid.h>
#include <hieros/gdt.h>
#include <hieros/cr.h>
#include <hieros/msr.h>
#include <hieros/paging.h>
#include <hieros/vga.h>

.section .rodata

.text
.section .text
.code16

.globl _start
_start:
	jmp _start

.section .text32
.code32

.section .text64
.code64
