#include <hieros/gdt.h>
#include <hieros/cr.h>
#include <hieros/msr.h>
#include <hieros/asm.h>
#include <hieros/trampoline.h>

.text
.code16
.globl TRAMPOLINE_SYM
TRAMPOLINE_SYM:
ASM_FUNC_BEGIN(__entry16)
	cli
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss

	lidt	NORELOC(__idt)
	lgdt	NORELOC(__gdt32_ptr)

	/* set protected mode */
	movl	%cr0, %eax
	orl	$CR0_PE, %eax
	movl	%eax, %cr0

	jmp	__flush_prefetch

__flush_prefetch:
	movw	$__KERNEL_DS, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss

	DATA32 ljmp $__KERNEL32_CS, $NORELOC_ABS(__entry32)
ASM_FUNC_END(__entry16)

.code32
ASM_FUNC_BEGIN(__entry32)
	jmp __entry32
	/* set PAE */
	movl	%cr4, %eax
	orl	$CR4_PAE, %eax
	movl	%eax, %cr4

	/* FIXME: setup identity page tables */
	//movl    $(__boot_pml4t - KERNEL_VMA), %edi
	movl	%edi, %cr3


	/* enable long mode */
	movl	$MSR_EFER, %ecx
	rdmsr
	orl	$EFER_LME, %eax
	wrmsr

	/* enable paging (cr0 PG + PE) */
	movl	$(CR0_PG | CR0_PE), %eax
	movl	%eax, %cr0

	/* TODO: use 64bit gdt */
	ljmp $__KERNEL_CS, $NORELOC_ABS(__entry64)
ASM_FUNC_END(__entry32)

.code64
ASM_FUNC_BEGIN(__entry64)
	movl	$__KERNEL_DS, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss

	cld
	callq	__ap_boot

	cli
.Lloop:
	rep	nop
	hlt
	jmp	.Lloop
ASM_FUNC_END(__entry64)

	.balign	8
ASM_VAR_BEGIN(__gdt32_ptr)
	.word	(__gdt32_end - __gdt32_ptr) - 1
	.long	NORELOC_ABS(__gdt32_ptr)
ASM_VAR_END(__gdt32_ptr)

	.balign	8
ASM_VAR_BEGIN(__gdt32)
	.quad	0x00cf9a000000ffff /* __KERNEL32_CS */
	.quad	0x00cf92000000ffff /* __KERNEL_DS */
ASM_VAR_END(__gdt32)
__gdt32_end:

	.balign	8
ASM_VAR_BEGIN(__idt)
	.word	0
	.long	0
ASM_VAR_END(__idt)

.globl TRAMPOLINE_END_SYM
TRAMPOLINE_END_SYM:
