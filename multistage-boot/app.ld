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

PROVIDE(_START_HEAP = _end);
PROVIDE(_END_HEAP = ORIGIN(SRAM) + LENGTH(SRAM));
PROVIDE(_END_STACK = ORIGIN(CCRAM) + LENGTH(CCRAM));
PROVIDE(STACK_SIZE = LENGTH(CCRAM));
