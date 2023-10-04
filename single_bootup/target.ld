MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256k
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 64k
}

SECTIONS
{
    .text : {
        _start_text = .;
        KEEP(*(.isr_vector)) /*IVT*/
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _end_text = .;

    } > FLASH

    _stored_data = .; /*Load Memory Address*/
    .data : AT(_stored_data)
    {
        _start_data = .; /*Virtual Memory Address*/
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

END_STACK = ORIGIN(RAM) + LENGTH(RAM);
