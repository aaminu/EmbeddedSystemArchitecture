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
        _start_text_bl = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _end_text_bl = .;
    } > FLASH

    _stored_data_bl = .;

    .data : AT(_stored_data_bl)
    {
        _start_data_bl = .;
        *(.data*)
        . = ALIGN(4);
        _end_data_bl = .;
    } > SRAM

    .bss (NOLOAD) : 
    {
        _start_bss_bl = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _end_bss_bl = .;
        _end_bl = .;
    } > SRAM
    . = ALIGN(4);
}

PROVIDE(_END_STACK = ORIGIN(SRAM) + LENGTH(SRAM));
