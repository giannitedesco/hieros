#include <hieros/multiboot.h>
#include <hieros/kaddr.h>
#include <hieros/serial.h>
#include <hieros/eflags.h>
#include <hieros/cpuid.h>
#include <hieros/gdt.h>
#include <hieros/cr.h>
#include <hieros/msr.h>
#include <hieros/asm.h>

.section .text
.text
.code64

ASM_FUNC_BEGIN(_start64)
	/* Move to kernel virtual address */
	addq	$KERNEL_VMA, %rsp
	pushq	$_high_start
	retq
ASM_FUNC_END(_start64)

ASM_LOCAL_FUNC_BEGIN(_high_start)
	movl	$__KERNEL_DS, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss

	/* Translate pointers in to the kernel identity map */
	addq	$KERNEL_VMA, __vga_vidptr
	addq	$KERNEL_VMA, __multiboot_info

	/* Reset the stack, ensuring 16-byte alignment, and call in to main
	 */
	movq	$(__boot_stack_top - 8), %rsp
	cld
	callq	kernel_main
ASM_FUNC_END(_high_start)

ASM_FUNC_BEGIN(hang)
	cli
.Lloop:
	rep	nop
	hlt
	jmp	.Lloop
ASM_FUNC_END(hang)

.macro PUSH_REGS
	pushq %r15
	movq %rsp, %r15 # store orig rsp
	addq $48, %r15 # calc offset
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %r11
	pushq %r10
	pushq %r9
	pushq %r8
	pushq %r15 /* orig rsp */
	pushq %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %rbx
	pushq %rax
.endm

.macro POP_REGS
	popq %rax
	popq %rbx
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi
	popq %rbp
	popq %r15 /* orig rsp */
	popq %r8
	popq %r9
	popq %r10
	popq %r11
	popq %r12
	popq %r13
	popq %r14
	popq %r15
.endm

.macro INTR sym, func, has_error_code:req
ASM_FUNC_BEGIN(\sym)
	.if \has_error_code == 0
	pushq $-1
	.endif

	PUSH_REGS

	# Pointer to regs stack as first argument
	movq %rsp, %rdi
	call \func

	POP_REGS

	addq $8, %rsp

	iretq

ASM_FUNC_END(\sym)
.endm

INTR spurious_intr do_spurious_intr has_error_code=0

INTR divide_error do_divide_error has_error_code=0
INTR invalid_opcode_error do_invalid_opcode_error has_error_code=0

INTR double_fault do_double_fault has_error_code=1
INTR tss_fault do_tss_fault has_error_code=1
INTR seg_fault do_seg_fault has_error_code=1
INTR ss_fault do_ss_fault has_error_code=1
INTR gpf do_gpf has_error_code=1
INTR pagefault do_pagefault has_error_code=1
INTR alignment_check do_alignment_check has_error_code=1

INTR lapic_spurious do_lapic_spurious has_error_code=0
INTR lapic_error do_lapic_error has_error_code=0
