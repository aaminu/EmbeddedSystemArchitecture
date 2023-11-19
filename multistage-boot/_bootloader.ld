MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 16K
    SRAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 128K
    CCRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 64K
}

SECTIONS
{
    .text :
    {
        _start_text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _end_text = .;
    } > FLASH

    _stored_data = .;

    .data : AT(_stored_data)
    {
        _start_data = .;
        *(.data*)
        . = ALIGN(4);
        _end_data = .;
    } > SRAM

    .bss (NOLOAD) : 
    {
        _start_bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _end_bss = .;
        _end = .;
    } > SRAM
    . = ALIGN(4);
}

PROVIDE(_END_STACK = ORIGIN(CCRAM) + LENGTH(CCRAM));
