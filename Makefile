# ==== CONFIG ====
ASM     = nasm
QEMU    = qemu-system-x86_64
IMG     = os.bin

# ==== RULES ====
all: $(IMG)
	@echo "✅ Build complete. Run with 'make run'"

# Assemble boot16.asm (boot sector, 512 bytes)
boot16.bin: boot16.asm
	$(ASM) -f bin $< -o $@

# Assemble stage2.asm (loaded by boot16loader)
stage2.bin: stage2.asm
	$(ASM) -f bin $< -o $@

# Combine boot sector and stage2 into one flat binary
$(IMG): boot16.bin stage2.bin
	cat boot16.bin stage2.bin > $(IMG)

# Run in QEMU
run: $(IMG)
	$(QEMU) -drive format=raw,file=$(IMG)

# Clean up
clean:
	rm -f *.bin $(IMG)
