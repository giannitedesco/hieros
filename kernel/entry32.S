#include <hieros/multiboot.h>
#include <hieros/kaddr.h>
#include <hieros/ioports.h>
#include <hieros/eflags.h>
#include <hieros/cpuid.h>
#include <hieros/gdt.h>
#include <hieros/asm.h>
#include <hieros/cr.h>
#include <hieros/msr.h>
#include <hieros/paging.h>
#include <hieros/vga.h>

.section .rodata
welcome:
	.ascii  "      _  _ _                  _             _\n"
	.ascii  " The | || (_)___ _ _ ___ _ __| |_  __ _ _ _| |_\n"
	.ascii  "     | __ | / -_) '_/ _ \\ '_ \\ ' \\/ _` | ' \\  _|\n"
	.ascii  "     |_||_|_\\___|_| \\___/ .__/_||_\\__,_|_||_\\__|\n"
	.ascii  "                        |_|      is awakening...\n"
	.ascii  "              Copyright (c) 2020 Gianni Tedesco.\n"
	.string "\n"
boot_failed:
	.string "ERROR: Early boot failure. Aborting OS load in 32bit mode.\n"
success:
	.string "Early boot complete, jumping to 64bit code.\n"
bad_magic:
	.string "Bad multiboot magic!\n"
no_ext_cpuid:
	.ascii  "Extended "
no_cpuid:
	.string "CPUID not supported!\n"
no_lm:
	.string "Long-mode not supported! Cannot switch to 64bit mode.\n"
goodbye:
	.string "Farewell\n"

.section .data
	.data

	.balign	8
ASM_VAR_BEGIN(gdt_ptr)
gdt_ptr:
	.word	(gdt_end - gdt) - 1
	.long	(gdt - KERNEL_VMA)
	.long	0
ASM_VAR_END(gdt_ptr)

	.balign	8
gdt:
	.quad	0
	.quad	0x00cf9a000000ffff /* __KERNEL32_CS */
	.quad	0x00af9a000000ffff /* __KERNEL_CS */
	.quad	0x00cf92000000ffff /* __KERNEL_DS */
gdt_end:

ASM_VAR_BEGIN(__multiboot_info)
__multiboot_info:
	.quad 0
ASM_VAR_END(__multiboot_info)

ASM_VAR_BEGIN(__vga_vidptr)
	.quad 0
ASM_VAR_END(__vga_vidptr)

.section .text
.text
.code32

ASM_FUNC_BEGIN(_start)
	jmp	multiboot_entry

.align	4
multiboot_header:
	.long	MULTIBOOT_HEADER_MAGIC
	.long	MULTIBOOT_HEADER_FLAGS
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

multiboot_entry:
	movl	$(__boot_stack_top - KERNEL_VMA), %esp

	/* Reset EFLAGS */
	pushl	$0
	popfl

	/* Initialise vga output pointer */
	movl	$VGA_BASE_ADDR, (__vga_vidptr - KERNEL_VMA)

	/* Multiboot:
	 * %eax : magic number
	 * %ebx : pointer to __multiboot_info structure
	 *
	 * Save these away now, we'll check them soon.
	 */
	movl	%ebx, (__multiboot_info - KERNEL_VMA)
	pushl	%eax

	/* First things first, ASCII art */
	movl	$(welcome - KERNEL_VMA), %eax
	call	__puts32

	/* Now check the multiboot magic */
	popl	%eax
	movl	$MULTIBOOT_BOOTLOADER_MAGIC, %ebx
	cmp	%eax, %ebx
	je	multiboot_ok
	movl	$(bad_magic - KERNEL_VMA), %eax
	jmp	die
multiboot_ok:

	/* check for CPUID support */
	movl	$EFLAGS_CPUID, %eax
	pushl	%eax
	popfl
	pushfl
	popl	%eax
	testl	$EFLAGS_CPUID, %eax
	jnz	cpuid_ok
	movl	$(no_cpuid - KERNEL_VMA), %eax
	jmp	die
cpuid_ok:

	/* Check for extended CPUID flags */
	movl	$0x80000000, %eax
	cpuid
	subl	$0x80000001, %eax
	cmpl	$(0x8000ffff - 0x80000001), %eax
	jbe	ext_cpuid_ok
	movl	$(no_ext_cpuid - KERNEL_VMA), %eax
	jmp	die
ext_cpuid_ok:

	/* Use CPUID to check long-mode supported */
	movl	$0x80000001, %eax
	cpuid
	btl	$(CPUID_LM_BIT), %edx
	jc	lm_ok
	movl	$(no_lm - KERNEL_VMA), %eax
	jmp	die
lm_ok:

	/* Setup our GDT and reload segment regs */
	movl	$(gdt_ptr - KERNEL_VMA), %eax
	lgdt	(%eax)
	ljmp	$__KERNEL32_CS, $1f
1:
	movl	$__KERNEL_DS, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs
	movl	%eax, %ss

	/* Setting up long mode is done in six steps as per vol 3, chapter
	 * 9.8.5 of the manual: "Initializing IA-32e Mode."
	 */

	/* 1. Disable paging: already not enabled.. */

	/* 2. Enable PAE */
	movl	%cr4, %eax
	orl	$CR4_PAE, %eax
	movl	%eax, %cr4

	/* 3. Setup 64bit paging: PML4T, PDPT, PDT. Here we'll just map the
	 * first 2MB in to the identity mapping from zero and from kernel vma
	 */

	/* point to PDPT, mark as present/rw */
	movl	$(__boot_pml4t - KERNEL_VMA), %edi
	movl	%edi, %cr3
	movl	$(__boot_pdpt - KERNEL_VMA), %eax
	orl	$(PML4E_PRESENT | PML4E_RW), %eax
	movl	%eax, (%edi)
	addl	$(511 * 8), %edi
	movl	%eax, (%edi)

	/* Point to PDT */
	movl	$(__boot_pdpt - KERNEL_VMA), %edi
	movl	$(__boot_pdt - KERNEL_VMA), %eax
	orl	$(PDPTE_PRESENT | PDPTE_RW), %eax
	movl	%eax, (%edi)

	addl	$(510 * 8), %edi
	movl	%eax, (%edi)

	/* Identity map first 1GB of RAM */
	xorl	%esi, %esi
	movl	$(PDTE_HUGE | PDTE_PRESENT | PDTE_RW), %eax
	movl	$(__boot_pdt - KERNEL_VMA), %edi
1:
	movl	%eax, (%edi)
	addl	$PAGE2M_SIZE, %eax
	incl	%esi
	addl	$8, %edi
	cmp	$512, %esi
	jb	1b


	/* 4. Enable long-mode */
	movl	$MSR_EFER, %ecx
	rdmsr
	orl	$EFER_LME, %eax
	wrmsr

	/* 5. enable paging (cr0 PG + PE) */
	movl	$(CR0_PG | CR0_PE), %eax
	movl	%eax, %cr0

	/* Print success message */
	movl	$(success - KERNEL_VMA), %eax
	call	__puts32

	/* Finally, ljmp to 64bit code */
	pushl	$__KERNEL_CS
	pushl	$(_start64 - KERNEL_VMA)
	lret

ret_from_main:
	movl	$(goodbye - KERNEL_VMA), %eax
die:
	call	__puts32
	movl	$(boot_failed - KERNEL_VMA), %eax
	call	__puts32
hang:
	rep
	nop
	hlt
	jmp	hang
ASM_FUNC_END(_start)

ASM_LOCAL_FUNC_BEGIN(__serio_puts)
	movl	$COM1, %edx
	movl	%eax, %ecx
	movzbl	(%ecx), %eax
	testb	%al, %al
	je	2f
1:
	outb	%al, %dx
	movzbl	1(%ecx), %eax
	incl	%ecx
	testb	%al, %al
	jne	1b
2:
	ret
ASM_FUNC_END(__serio_puts)

ASM_LOCAL_FUNC_BEGIN(__vga_newline)
	pushl	%ebx
	movl	(__vga_vidptr - KERNEL_VMA), %eax
	leal	-753506(%eax), %ecx
	movl	%ecx, %eax
	movl	$160, %ebx
	movl	$0, %edx
	divl	%ebx
	subl	%edx, %ecx
	addl	$0xb8000, %ecx
	movl	%ecx, (__vga_vidptr - KERNEL_VMA)
	popl	%ebx
	ret
ASM_FUNC_END(__vga_newline)

ASM_LOCAL_FUNC_BEGIN(__vga_puts)
	pushl	%ebx
	movl	%eax, %ebx
	jmp	.Lloop
.Lnewline:
	call	__vga_newline
.Lnext:
	incl	%ebx
.Lloop:
	movzbl	(%ebx), %edx
	testb	%dl, %dl
	je	.Lout
	cmpb	$10, %dl
	je	.Lnewline
	movl	(__vga_vidptr - KERNEL_VMA), %eax
	movb	%dl, (%eax)
	movb	$0x0f, 1(%eax)
	addl	$2, %eax
	movl	%eax, (__vga_vidptr - KERNEL_VMA)
	jmp	.Lnext

.Lout:
	popl	%ebx
	ret
ASM_FUNC_END(__vga_puts)

ASM_LOCAL_FUNC_BEGIN(__puts32)
	pushl	%eax
	call	__serio_puts
	popl	%eax
	call	__vga_puts
	ret
ASM_FUNC_END(__puts32)

.section .bss
	.align 4096

ASM_LOCAL_VAR_BEGIN(stack)
	.skip 4096
ASM_VAR_END(stack)

ASM_VAR_BEGIN(__boot_stack_top)
ASM_VAR_END(__boot_stack_top)

ASM_VAR_BEGIN(__boot_pml4t)
	.skip 4096
ASM_VAR_END(__boot_pml4t)

ASM_VAR_BEGIN(__boot_pdpt)
	.skip 4096
ASM_VAR_END(__boot_pdpt)

ASM_VAR_BEGIN(__boot_pdt)
	.skip 4096
ASM_VAR_END(__boot_pdt)
