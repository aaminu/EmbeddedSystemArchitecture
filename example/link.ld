MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256k
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 64k
}

SECTIONS
{
    /* Text Section (Cdoe and read-only data) */
    .text :
    {
        . = ALIGN(4) ;
        _start_text = . ;
        *(.text*) /* code */
        . = ALIGN(4) ;
        _end_text = . ;
        *(.rodata*) /* read-only data */
        . = ALIGN(4) ;
        _end_rodata = . ;
    } > FLASH

    _stored_data = . ;
    .data : AT(_stored_data)
    {
        . = ALIGN(4) ;
        _start_data = . ;
        *(.data*)
        . = ALIGN(4) ;
        _end_data = . ;
    } > RAM

    .bss (NOLOAD) :
    {
        . = ALIGN(4) ;
        _start_bss = . ;
        *(.bss*)
        . = ALIGN(4) ;
        _end_bss = . ;
    } > RAM

    .keys :
    {
        . = ALIGN(4) ;
        _start_keys = . ; 
        *(.keys*)
        . = ALIGN(4) ;
        _end_keys = . ;
    } > FLASH
}
