# Makefile for x86_64 CLI OS

# Tools
ASM = nasm
CC = x86_64-elf-gcc
LD = x86_64-elf-ld
OBJCOPY = x86_64-elf-objcopy

# Flags
CFLAGS = -ffreestanding -nostdlib -fno-builtin -fno-stack-protector \
         -m64 -mno-red-zone -mcmodel=large -Wall -Wextra \
         -I kernel/lib
LDFLAGS = -T kernel/linker.ld -nostdlib

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel

# Source files - FIXED: separate ASM and C interrupt sources
KERNEL_C_SOURCES = $(wildcard $(KERNEL_DIR)/*.c) \
                   $(wildcard $(KERNEL_DIR)/drivers/*.c) \
                   $(wildcard $(KERNEL_DIR)/lib/*.c) \
                   $(wildcard $(KERNEL_DIR)/interrupts/*.c)

# ASM sources - separate list
KERNEL_ASM_SOURCES = $(KERNEL_DIR)/kernel_entry.asm \
                     $(KERNEL_DIR)/interrupts/isr.asm

# Object files
KERNEL_C_OBJS = $(KERNEL_C_SOURCES:.c=.o)
KERNEL_ASM_OBJS = $(KERNEL_ASM_SOURCES:.asm=_asm.o)
KERNEL_OBJS = $(KERNEL_ASM_OBJS) $(KERNEL_C_OBJS)

# Output files
BOOT_BIN = $(BOOT_DIR)/boot16.bin
STAGE2_BIN = $(BOOT_DIR)/boot32.bin
KERNEL_ELF = $(KERNEL_DIR)/kernel.elf
KERNEL_BIN = $(KERNEL_DIR)/kernel.bin
OS_IMG = os.img

QEMU = qemu-system-x86_64

# Phony targets
.PHONY: all clean run debug info

# Default target
all: $(OS_IMG)

# Assemble bootloader
$(BOOT_BIN): $(BOOT_DIR)/boot16.asm
	$(ASM) -f bin $< -o $@

# Assemble stage 2
$(STAGE2_BIN): $(BOOT_DIR)/boot32.asm
	$(ASM) -f bin $< -o $@

# Assemble kernel ASM files - FIXED: use _asm.o suffix to avoid conflicts
$(KERNEL_DIR)/%_asm.o: $(KERNEL_DIR)/%.asm
	$(ASM) -f elf64 $< -o $@

$(KERNEL_DIR)/interrupts/%_asm.o: $(KERNEL_DIR)/interrupts/%.asm
	$(ASM) -f elf64 $< -o $@

# Compile C files
$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_DIR)/drivers/%.o: $(KERNEL_DIR)/drivers/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_DIR)/lib/%.o: $(KERNEL_DIR)/lib/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_DIR)/interrupts/%.o: $(KERNEL_DIR)/interrupts/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel to ELF
$(KERNEL_ELF): $(KERNEL_OBJS) $(KERNEL_DIR)/linker.ld
	$(LD) $(LDFLAGS) $(KERNEL_OBJS) -o $@

# Convert ELF to flat binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# Create disk image
$(OS_IMG): $(BOOT_BIN) $(STAGE2_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(STAGE2_BIN) > $(OS_IMG)
	truncate -s $$((512 * 33)) $(OS_IMG)
	cat $(KERNEL_BIN) >> $(OS_IMG)
	truncate -s 1M $(OS_IMG)

# Run in QEMU
run: $(OS_IMG)
	$(QEMU) -drive format=raw,file=$(OS_IMG)

# Run with debugging
debug: $(OS_IMG)
	$(QEMU) -drive format=raw,file=$(OS_IMG) -d int,cpu_reset -no-reboot -no-shutdown

# Show kernel info
info: $(KERNEL_ELF) $(KERNEL_BIN)
	@echo "=== Kernel ELF Info ==="
	x86_64-elf-readelf -h $(KERNEL_ELF)
	@echo ""
	@echo "=== Kernel Entry Point ==="
	x86_64-elf-nm $(KERNEL_ELF) | grep _start
	@echo ""
	@echo "=== Kernel Binary Size ==="
	ls -lh $(KERNEL_BIN)
	@echo ""
	@echo "=== Object Files ==="
	@echo "$(KERNEL_OBJS)"

# Clean
clean:
	rm -f $(BOOT_BIN) $(STAGE2_BIN) $(KERNEL_C_OBJS) $(KERNEL_ASM_OBJS) $(KERNEL_ELF) $(KERNEL_BIN) $(OS_IMG)