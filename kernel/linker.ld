/* kernel/linker.ld - Linker script for 64-bit kernel */

ENTRY(_start)

SECTIONS
{
    /* Kernel loads at 1MB physical address */
    . = 0x100000;
    
    .text : ALIGN(4096)
    {
        *(.text)
    }
    
    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }
    
    .data : ALIGN(4096)
    {
        *(.data)
    }
    
    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)
    }
}