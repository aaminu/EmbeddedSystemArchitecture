#include <stdint.h>

/* Pointers to memory in RAM for different symbols */
extern uint32_t _stored_data; // Load memory area on Flash
extern uint32_t _start_data;
extern uint32_t _end_data;
extern uint32_t _start_bss;
extern uint32_t _end_bss;

extern uint32_t _END_STACK;
extern uint32_t _START_HEAP;

static uint32_t sp;

extern void main(void);

void isr_reset(void)
{
    uint32_t *src, *dst;

    /*Initialized data copy (.data)*/
    src = &_stored_data;
    dst = &_start_data;

    while (dst != &_end_data)
    {
        *dst++ = *src++;
    }

    /*zero out uninitilized values*/
    dst = &_start_bss;
    while (dst != &_end_bss)
    {
        *dst++ = 0u;
    }

/* Stack Painting*/
#ifdef STACK_PAINTING
    {
        asm volatile("mrs %0, msp" : "=r"(sp));
        dst = ((uint32_t *)(&_END_STACK)) - ((8 * 1024) / sizeof(uint32_t));
        while ((uint32_t)dst < sp)
        {
            *dst = 0xDEADC0DE;
            dst++;
        }
    }
#endif

    /*Run the main */
    main();
}

void isr_fault(void)
{
    /* Panic and hold continously */
    while (1)
        ;
    ;
}

void isr_empty(void)
{
    /* Ignore the event and continue */
}

__attribute__((section(".isr_vector"))) void (*const IV[])(void) = {

    (void (*)(void))(&_END_STACK), // Stack Pointer
    isr_reset,                     // Reset
    isr_fault,                     // Non maskable interrupt
    isr_fault,                     // Hard fault
    isr_fault,                     // Memory fault
    isr_fault,                     // Bus Fault
    isr_fault,                     // Usage Fault
    0, 0, 0, 0,                    // REserved
    isr_empty,                     // Supervisor call
    isr_empty,                     // Debug Monitor event
    0,                             // REserved
    isr_empty,                     // PendSV call
    isr_empty,                     // Systemtick
    isr_empty,                     // GPIO Port B
    isr_empty,                     // GPIO Port B
    isr_empty,                     // GPIO Port B
    isr_empty,                     // GPIO Port A
    isr_empty,                     // GPIO Port C
    isr_empty,                     // GPIO Port D
    isr_empty,                     // GPIO Port E
    isr_empty,                     // UART0 Rx and Tx
    isr_empty,                     // UART1 Rx and Tx
    isr_empty,                     // SSI0 Rx and Tx
    isr_empty,                     // I2C0 Master and Slave
    isr_empty,                     // PWM Fault
    isr_empty,                     // PWM Generator 0
    isr_empty,                     // PWM Generator 1
    isr_empty,                     // PWM Generator 2
    isr_empty,                     // Quadrature Encoder 0
    isr_empty,                     // ADC Sequence 0
    isr_empty,                     // ADC Sequence 1
    isr_empty,                     // ADC Sequence 2
    isr_empty,                     // ADC Sequence 3
    isr_empty,                     // Watchdog timer
    isr_empty,                     // Timer 0 subtimer A
    isr_empty,                     // Timer 0 subtimer B
    isr_empty,                     // Timer 1 subtimer A
    isr_empty,                     // Timer 1 subtimer B
    isr_empty,                     // Timer 2 subtimer A
    isr_empty,                     // Timer 3 subtimer B
    isr_empty,                     // Analog Comparator 0
    isr_empty,                     // Analog Comparator 1
    isr_empty,                     // Analog Comparator 2
    isr_empty,                     // System Control (PLL, OSC, BO)
    isr_empty,                     // FLASH Control
    isr_empty,                     // GPIO Port F
    isr_empty,                     // GPIO Port G
    isr_empty,                     // GPIO Port H
    isr_empty,                     // UART2 Rx and Tx
    isr_empty,                     // SSI1 Rx and Tx
    isr_empty,                     // Timer 3 subtimer A
    isr_empty,                     // Timer 3 subtimer B
    isr_empty,                     // I2C1 Master and Slave
    isr_empty,                     // Quadrature Encoder 1
    isr_empty,                     // CAN0
    isr_empty,                     // CAN1
    isr_empty,                     // CAN2
    isr_empty,                     // Ethernet
    isr_empty,                     // Hibernate
};
