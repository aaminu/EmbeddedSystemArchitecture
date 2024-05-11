MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08004000, LENGTH = 1008K
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

    .data : AT(_stored_data) /*LMA*/
    {
        _start_data = .; /*VMA*/
        *(.data*)
        . = ALIGN(4);
        _end_data = .;
    } > SRAM

    .bss : 
    {
        _start_bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _end_bss = .;
        _end = .;
    } > SRAM
}

__HEAP = 20K;
PROVIDE(_START_HEAP = _end_bss);
PROVIDE(_END_HEAP = _START_HEAP + __HEAP);
PROVIDE(_END_STACK = ORIGIN(SRAM) + LENGTH(SRAM));
PROVIDE(_FLASH = _start_text);
PROVIDE(_SRAM = _start_data) ;
