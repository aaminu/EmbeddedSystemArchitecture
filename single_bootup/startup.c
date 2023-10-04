
/* Pointers to memory in RAM for different symbols */
extern unsigned int _stored_data; // Load memory area on Flash
extern unsigned int _start_data;
extern unsigned int _end_data;
extern unsigned int _start_bss;
extern unsigned int _end_bss;

extern void *END_STACK;

static int zeroed_variable_in_bss;
static int initialized_variable_in_data = 42;

void main(void);

void isr_reset(void)
{
    // Move initialized data to RAM from LMA
    unsigned int *src, *dst;
    src = (unsigned int *)&_stored_data;
    dst = (unsigned int *)&_start_data;

    while (dst != (unsigned int *)&_end_data)
    {
        *dst++ = *src++;
    }

    // Zero out BSS section
    for (dst = (unsigned int *)&_start_bss; dst < (unsigned int *)&_end_bss; dst++)
    {
        *dst = 0u;
    }

    main();
}

void isr_fault(void)
{
    /* Panics*/
    while (1)
        ;
    ;
}

void isr_empty(void)
{
}

void main(void)
{
    while (1)
    {
        zeroed_variable_in_bss++;
        initialized_variable_in_data++;
        // Emulate a delay
        for (unsigned int i; i < 150000; i++)
            ;
    }
}

/**
 * Interrupt Vector Table
 * Start of Image, Intial SP and other routine
 */
__attribute__((section(".isr_vector"))) void (*const IV[])(void) =
    {
        (void (*)(void))(&END_STACK), // Stack Pointer
        isr_reset,                    // Reset
        isr_fault,                    // Non maskable interrupt
        isr_fault,                    // Hard fault
        isr_fault,                    // Memory fault
        isr_fault,                    // Bus Fault
        isr_fault,                    // Usage Fault
        0, 0, 0, 0,
        isr_empty, // Supervisor call
        isr_empty, // Debug Monitor event
        0,
        isr_empty, // PendSV call
        isr_empty, // Systemtick
        /*Peripherals*/
        isr_empty, // GPIO Port B
        isr_empty, // GPIO Port B
        isr_empty, // GPIO Port B
        isr_empty, // GPIO Port A
        isr_empty, // GPIO Port C
        isr_empty, // GPIO Port D
        isr_empty, // GPIO Port E
        isr_empty, // UART0 Rx and Tx
        isr_empty, // UART1 Rx and Tx
        isr_empty, // SSI0 Rx and Tx
        isr_empty, // I2C0 Master and Slave
        isr_empty, // PWM Fault
        isr_empty, // PWM Generator 0
        isr_empty, // PWM Generator 1
        isr_empty, // PWM Generator 2
        isr_empty, // Quadrature Encoder 0
        isr_empty, // ADC Sequence 0
        isr_empty, // ADC Sequence 1
        isr_empty, // ADC Sequence 2
        isr_empty, // ADC Sequence 3
        isr_empty, // Watchdog timer
        isr_empty, // Timer 0 subtimer A
        isr_empty, // Timer 0 subtimer B
        isr_empty, // Timer 1 subtimer A
        isr_empty, // Timer 1 subtimer B
        isr_empty, // Timer 2 subtimer A
        isr_empty, // Timer 3 subtimer B
        isr_empty, // Analog Comparator 0
        isr_empty, // Analog Comparator 1
        isr_empty, // Analog Comparator 2
        isr_empty, // System Control (PLL, OSC, BO)
        isr_empty, // FLASH Control
        isr_empty, // GPIO Port F
        isr_empty, // GPIO Port G
        isr_empty, // GPIO Port H
        isr_empty, // UART2 Rx and Tx
        isr_empty, // SSI1 Rx and Tx
        isr_empty, // Timer 3 subtimer A
        isr_empty, // Timer 3 subtimer B
        isr_empty, // I2C1 Master and Slave
        isr_empty, // Quadrature Encoder 1
        isr_empty, // CAN0
        isr_empty, // CAN1
        isr_empty, // CAN2
        isr_empty, // Ethernet
        isr_empty, // Hibernate
};

/**
 * Cortex M3/M4 has Configurable Fault Status Register Mapped at 0xE000ED28
 * The Reset Handler has to be at the 0x04 and SP at 0x00
 */
