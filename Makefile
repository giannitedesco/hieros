MAKEFLAGS += --no-builtin-rules
.SUFFIXES:
.SECONDEXPANSION:

TARGET: all

DIST_ISO := hieros.iso
KVM_FLAGS := -cpu max
KVM_FLAGS := -cpu host -enable-kvm
QEMU_FLAGS := \
		-smp 2 \
		-m 1024 \
		-boot d -cdrom $(DIST_ISO) -nic none \
		-d int,mmu,pcall,cpu_reset,guest_errors -no-reboot \
		$(KVM_FLAGS)

OBJ_DIR := obj
IMG_DIR := img

.PHONY: all
all: image

.PHONY: clean
clean:
	rm -rf obj img hieros.iso

ifdef V
MAKEFLAGS += --verbose
else
MAKEFLAGS += --quiet
endif

MIN_CPU ?= core2
TARGET_CPU ?= native

#DEBUG_OPTS := -Og -fno-inline-functions \

CC := gcc
CFLAGS := \
	-march=$(MIN_CPU) \
	-mtune=$(TARGET_CPU) \
	-mno-red-zone \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mcmodel=large \
	\
	-O2 \
	-flto -fwhole-program -fno-fat-lto-objects \
	-finline-functions \
	-ftree-partial-pre \
	-fgcse-after-reload \
	-fipa-cp-clone \
	-fipa-pta \
	$(DEBUG_OPTS) \
	\
	-Wall \
	-Wsign-compare \
	-Wcast-align \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-Wmissing-declarations \
	-Wmissing-noreturn \
	-Wmissing-format-attribute \
	-Wmaybe-uninitialized \
	-Wlogical-op \
	-Wduplicated-cond \
	-Wduplicated-branches \
	-Wlogical-op \
	-Wrestrict \
	-Wnull-dereference \
	-Wimplicit-fallthrough \
	-Wjump-misses-init \
	-Wswitch-enum \
	\
	-static \
	-static-libgcc \
	-nostdlib \
	-nostdinc \
	-ffreestanding \
	-z max-page-size=0x1000 \
	-fno-pie -fno-PIE \
	\
	-Iinclude

one_dep = $(patsubst %.o,%.d,$1)
depfile = $(foreach obj,$1,$(call one_dep,$(obj)))

dstamp = $1/dir-stamp

%/dir-stamp:
	@mkdir -p $(dir $@)
	@touch $@

$(OBJ_DIR)/%.o: %.c $(MAKEFILE_LIST) | $(call dstamp,$$(@D))
	$(info compile: $@)
	$(CC) $(CFLAGS) \
		-MMD -MP -MF $(call one_dep,$@) -MT $@ -c -o $@ $<

$(OBJ_DIR)/%.o: %.S $(MAKEFILE_LIST) | $(call dstamp,$$(@D))
	$(info asm: $@)
	$(CC) $(CFLAGS) -D__ASM__ \
		-MMD -MP -MF $(call one_dep,$@) -MT $@ -c -o $@ $<

KLIB_OBJ := $(OBJ_DIR)/kernel/lib/printf.o
KERNEL_OBJ := \
	$(OBJ_DIR)/kernel/entry32.o \
	$(OBJ_DIR)/kernel/entry64.o \
	$(OBJ_DIR)/kernel/bootmem.o \
	$(OBJ_DIR)/kernel/main.o \
	$(OBJ_DIR)/kernel/cpuid.o \
	$(OBJ_DIR)/kernel/ram_map.o \
	$(OBJ_DIR)/kernel/lapic.o \
	$(OBJ_DIR)/kernel/faults.o \
	$(OBJ_DIR)/kernel/idt.o \
	$(OBJ_DIR)/kernel/acpi.o \
	$(OBJ_DIR)/kernel/printk.o \
	$(OBJ_DIR)/kernel/serio.o \
	$(OBJ_DIR)/kernel/vga.o \
	$(KLIB_OBJ)
ALL_OBJ := $(KERNEL_OBJ)

ALL_DEP := $(call depfile,$(ALL_OBJ)) $(EXTRA_DEPS)

ifneq ($(filter clean, $(MAKECMDGOALS)), clean)
$(debug including $(ALL_DEP))
-include $(ALL_DEP)
endif

KLINK := kernel/kernel.lnk
$(OBJ_DIR)/kernel.elf: $(MAKEFILE_LIST) $(KERNEL_OBJ) $(KLINK)
	$(info elf: $@)
	$(CC) $(CFLAGS) \
		-Wl,-nostdlib \
		-nostartfiles \
		-Wl,-z,execstack \
		-Wl,--build-id=none \
		-Wl,-T,$(KLINK) -o $@ \
		$(KERNEL_OBJ) -nostdlib

.PHONY: run
run: image
	$(info *** The hierophant awakens ***)
	qemu-system-x86_64 -serial stdio $(QEMU_FLAGS)

.PHONY: serial
serial: image
	$(info *** The hierophant awakens ***)
	qemu-system-x86_64 $(QEMU_FLAGS) \
		-nographic -serial stdio -display none -monitor none \

.PHONY: image
image: $(DIST_ISO)
	$(info *** The hierophant beckons ***)

$(DIST_ISO): $(OBJ_DIR)/kernel.elf | $(IMG_DIR)/boot/grub/dir-stamp
	$(info iso: $@)
	cp -lf $(OBJ_DIR)/kernel.elf $(IMG_DIR)/boot/hieros.elf
	cp -lf boot/grub.cfg $(IMG_DIR)/boot/grub/grub.cfg
	grub2-mkrescue -o hieros.iso $(IMG_DIR) 2> /dev/null
