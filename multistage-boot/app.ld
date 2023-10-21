MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00001000, LENGTH = 252k
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 64k
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
    } > RAM

    .bss : 
    {
        _start_bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _end_bss = .;
        _end = .;
    } > RAM
}

_START_HEAP = _end;
_END_STACK = ORIGIN(RAM) + LENGTH(RAM);
PROVIDE(STACK_SIZE = (4 * 1024));
PROVIDE(GUARD_SIZE = 1024);
