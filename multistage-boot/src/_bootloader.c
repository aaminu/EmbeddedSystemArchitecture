#include <stdint.h>

// Vector Table Offset Register Address
#define APP_OFFSET (0x08004000) // Application partition offset
#define BOOTLOADER

/* symbols for memory locations in declared in linker */
extern uint32_t _stored_data; // Load memory area on Flash
extern uint32_t _start_data;
extern uint32_t _end_data;
extern uint32_t _start_bss;
extern uint32_t _end_bss;
extern uint32_t _END_STACK;

void main_bl(void);

void isr_reset_bl(void)
{
    volatile uint32_t *src, *dst;

    /*Initialized data copy (.data)*/
    src = (uint32_t *)&_stored_data;
    dst = (uint32_t *)&_start_data;

    while (dst != (uint32_t *)&_end_data)
    {
        *dst = *src;
        dst++;
        src++;
    }

    /*zero out uninitilized values*/
    dst = (uint32_t *)&_start_bss;
    while (dst != (uint32_t *)&_end_bss)
    {
        *dst = 0u;
        dst++;
    }

    /*Run the main */
    main_bl();
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

void main_bl()
{
    void (*app_entry)(void);
    uint32_t app_end_stack;

    /*Disable all interrupts*/
    asm volatile("cpsid i");

    /*update VTOR; with Offset to the Application IVT start*/
    volatile uint32_t *vtor = (uint32_t *)0xE000ED08;
    *vtor = ((uint32_t)APP_OFFSET & 0xFFFFFFF8);

    /*get stack point and entry function(isr_reset)*/
    app_end_stack = (*((uint32_t *)(APP_OFFSET)));
    app_entry = (void (*)(void))(*(uint32_t *)(APP_OFFSET + 4));

    /*Update stack pointer*/
    /*MSR to move value from normal register to special register*/
    asm volatile("msr msp, %0" ::"r"(app_end_stack));

    /*Enable all interrupts*/
    asm volatile("cpsie i");

    /*Jump to the app entry function(isr_reset)*/
    asm volatile("mov pc, %0" ::"r"(app_entry));
}

__attribute__((section(".isr_vector"))) void (*const IV[])(void) = {

    (void (*)(void))(&_END_STACK), // Stack Pointer
    isr_reset_bl,                  // Reset
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

/*
*asm asm-qualifiers ( AssemblerTemplate
                      : OutputOperands
                      : InputOperands
                      : Clobbers
                      : GotoLabels)
*/
