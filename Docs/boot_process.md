```markdown
1. BIOS loads bootloader from 0x7C00
2. While in stage 1 (16-bit Real Mode)
    - Prepare stack and segments
    - Store boot_driver info
    - Reset disk to prevent errors
    - Get and store RAM size for kernel to use
    - Load stage 2 into 0000:1000 using LBA
    - Load kernel into 0x10000 using LBA
    - Jump to stage 2
3. While in stage 2
    - Enable A20
    - Load GDT
    - Enter 32-bit Protected Mode
    - Migrate stack and segments from 16-bit to 32-bit
        Prior to this change selectors are indexed 0 for safety reasons.
        To make them useable we change them to our liking.
    - Copy kernel from 0x10000 to 0x100000 (1MB for safety reasons)
    - Check if CPU supports 64-bit Long Mode (If not, halt)
    - Set up paging
    - Enable PAE (Physical Address Extension)
    - Load PML4 address into CR3
    - Enable 64-bit Long Mode
    - Load GDT
    - Jump to 64-bit Long Mode
    - Clear segment registers
    - Set up 64-bit stack
    - Jump to Kernel 0x100000 (1MB)


There exists error handling code right after every interupt call