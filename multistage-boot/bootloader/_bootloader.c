#include <stdint.h>

// Vector Table Offset Register Address
#define APP_OFFSET (0x08004000) // Application partition offset
#define BOOTLOADER

/* symbols for memory locations in declared in linker */
extern uint32_t _stored_data_bl; // Load memory area on Flash
extern uint32_t _start_data_bl;
extern uint32_t _end_data_bl;
extern uint32_t _start_bss_bl;
extern uint32_t _end_bss_bl;
extern uint32_t _END_STACK;

void main_bl(void);

void isr_reset_bl(void)
{
    volatile uint32_t *src, *dst;

    /*Initialized data copy (.data)*/
    src = (uint32_t *)&_stored_data_bl;
    dst = (uint32_t *)&_start_data_bl;

    while (dst != (uint32_t *)&_end_data_bl)
    {
        *dst = *src;
        dst++;
        src++;
    }

    /*zero out uninitilized values*/
    dst = (uint32_t *)&_start_bss_bl;
    while (dst != (uint32_t *)&_end_bss_bl)
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
    isr_empty,                     // WWDG
    isr_empty,                     // PVD
    isr_empty,                     // TAMP_STAMP
    isr_empty,                     // RTC _WKUP
    isr_empty,                     // FLASH
    isr_empty,                     // RCC
    isr_empty,                     // EXTI0
    isr_empty,                     // EXTI1
    isr_empty,                     // EXTI2
    isr_empty,                     // EXTI3
    isr_empty,                     // EXTI4
    isr_empty,                     // DMA1_Stream0
    isr_empty,                     // DMA1_Stream1
    isr_empty,                     // DMA1_Stream2
    isr_empty,                     // DMA1_Stream3
    isr_empty,                     // DMA1_Stream4
    isr_empty,                     // DMA1_Stream5
    isr_empty,                     // DMA1_Stream6
    isr_empty,                     // ADC
    isr_empty,                     // CAN1_TX
    isr_empty,                     // CAN1_RX0
    isr_empty,                     // CAN1_RX1
    isr_empty,                     // CAN1_SCE
    isr_empty,                     // EXTI9_5
    isr_empty,                     // TIM1_BRK_TIM9
    isr_empty,                     // TIM1_UP_TIM10
    isr_empty,                     // TIM1_TRG_COM_TIM11
    isr_empty,                     // TIM1_CC
    isr_empty,                     // TIM2
    isr_empty,                     // TIM3
    isr_empty,                     // TIM4
    isr_empty,                     // I2C1_EV
    isr_empty,                     // I2C1_ER
    isr_empty,                     // I2C2_EV
    isr_empty,                     // I2C2_ER
    isr_empty,                     // SPI1
    isr_empty,                     // SPI2
    isr_empty,                     // USART1
    isr_empty,                     // USART2
    isr_empty,                     // USART3
    isr_empty,                     // EXTI15_10
    isr_empty,                     // RTC_Alarm
    isr_empty,                     // OTG_FS_WKUP
    isr_empty,                     // TIM8_BRK_TIM12
    isr_empty,                     // TIM8_UP_TIM13
    isr_empty,                     // TIM8_TRG_COM_TIM14
    isr_empty,                     // TIM8_CC
    isr_empty,                     // DMA1_Stream7
    isr_empty,                     // FSMC
    isr_empty,                     // SDIO
    isr_empty,                     // TIM5
    isr_empty,                     // SPI3
    isr_empty,                     // UART4
    isr_empty,                     // UART5
    isr_empty,                     // TIM6_DAC
    isr_empty,                     // TIM7
    isr_empty,                     // DMA2_Stream0
    isr_empty,                     // DMA2_Stream1
    isr_empty,                     // DMA2_Stream2
    isr_empty,                     // DMA2_Stream3
    isr_empty,                     // DMA2_Stream4
    isr_empty,                     // ETH
    isr_empty,                     // ETH_WKUP
    isr_empty,                     // CAN2_TX
    isr_empty,                     // CAN2_RX0
    isr_empty,                     // CAN2_RX1
    isr_empty,                     // CAN2_SCE
    isr_empty,                     // OTG_FS
    isr_empty,                     // DMA2_Stream5
    isr_empty,                     // DMA2_Stream6
    isr_empty,                     // DMA2_Stream7
    isr_empty,                     // USART6
    isr_empty,                     // I2C3_EV
    isr_empty,                     // I2C3_ER
    isr_empty,                     // OTG_HS_EP1_OUT
    isr_empty,                     // OTG_HS_EP1_IN
    isr_empty,                     //  OTG_HS_WKUP
    isr_empty,                     // OTG_HS
    isr_empty,                     //  DCMI
    isr_empty,                     // CRYP
    isr_empty,                     // HASH_RNG
    isr_empty,                     // FPU
};

/*
*asm asm-qualifiers ( AssemblerTemplate
                      : OutputOperands
                      : InputOperands
                      : Clobbers
                      : GotoLabels)
*/
