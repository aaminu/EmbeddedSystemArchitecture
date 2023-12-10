#include <stdint.h>
#include "systick.h"

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
extern void _isr_exti0(void);
extern void _isr_exti1(void);
extern void _isr_exti2(void);
extern void _isr_exti3(void);
extern void _isr_exti4(void);
extern void _isr_exti9_5(void);
extern void _isr_exti15_10(void);

void isr_reset(void)
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
    systick_isr,                   // Systemtick
    isr_empty,                     // WWDG
    isr_empty,                     // PVD
    isr_empty,                     // TAMP_STAMP
    isr_empty,                     // RTC _WKUP
    isr_empty,                     // FLASH
    isr_empty,                     // RCC
    _isr_exti0,                    // EXTI0
    _isr_exti1,                    // EXTI1
    _isr_exti2,                    // EXTI2
    _isr_exti3,                    // EXTI3
    _isr_exti4,                    // EXTI4
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
    _isr_exti9_5,                  // EXTI9_5
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
    _isr_exti15_10,                // EXTI15_10
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
