#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>

/*******RCC*******/
#define RCC_BASE (0x40023800)

/* Assembly helpers */
#define DMB() __asm__ volatile("dmb");
#define WFI() __asm__ volatile("wfi");
#define DSB() __asm__ volatile("dsb");
#define ISB() __asm__ volatile("isb");

/* System specific: PLL with 8 MHz external oscillator, CPU at 168MHz */
#define CPU_FREQ (168000000)
#define AHB1_FREQ (CPU_FREQ)
#define APB1_FREQ (42000000)
#define APB2_FREQ (84000000)
#define PLL_FULL_MASK (0x7F037FFF)

/*NVIC - Nested Interrupt Vector Controller for Cortex-M*/
/**
 * @brief Enable interrupts Base Address - Interrupt Set Enable Registers
 */
#define NVIC_ISER_BASE (0xE000E100)
/**
 * @brief Disable interrupts Base Address - Interrupt Clear Enable Registers
 */
#define NVIC_ICER_BASE (0xE000E180)
/**
 * @brief Pend an interrupt Base Address - Interrupt Set Pending registers
 */
#define NVIC_ISPR_BASE (0xE000E200)
/**
 * @brief Clear a pending interrupt Base Address - Interrupt Clear Pending registers
 */
#define NVIC_ICPR_BASE (0xE000E280)
/**
 * @brief Get the interrupt status Base Address - Interrupt Active Bit registers
 */
#define NVIC_IABR_BASE (0xE000E300)
/**
 * @brief Define the priority level of an interrupt Base Address - Interrupt Priority Registers
 */
#define NVIC_IPR_BASE (0xE000E400)

/**
 * @brief Enables an external interrupt
 * @param irqn - Interrupt Number
 */
inline void nvic_enable_irq(uint8_t irqn)
{
    uint8_t i = irqn / 32;
    volatile uint32_t *nvic_serenax = (volatile uint32_t *)(NVIC_ISER_BASE + (4 * i));
    *nvic_serenax |= (1 << (irqn % 32));
}

/**
 * @brief Disables an external interrupt
 * @param irqn - Interrupt Number
 */
inline void nvic_disable_irq(uint8_t irqn)
{
    uint8_t i = irqn / 32;
    volatile uint32_t *nvic_clrenax = (volatile uint32_t *)(NVIC_ICER_BASE + (4 * i));
    *nvic_clrenax |= (1 << (irqn % 32));
}

/**
 * @brief Sets the pending state of an interrupt
 * @param irqn - Interrupt Number
 */
inline void nvic_set_pending_irq(uint8_t irqn)
{
    uint8_t i = irqn / 32;
    volatile uint32_t *nvic_setpendx = (volatile uint32_t *)(NVIC_ISPR_BASE + (4 * i));
    *nvic_setpendx |= (1 << (irqn % 32));
}

/**
 * @brief Clears the pending state of an interrupt
 * @param irqn - Interrupt Number
 */
inline void nvic_clear_pending_irq(uint8_t irqn)
{
    uint8_t i = irqn / 32;
    volatile uint32_t *nvic_clrpendx = (volatile uint32_t *)(NVIC_ICPR_BASE + (4 * i));
    *nvic_clrpendx |= (1 << (irqn % 32));
}

/**
 * @brief Set the Priority of a Peripheral
 * @param irqn - Interrupt Number
 * @param prio - Priority Level
 */
inline void nvic_set_prio_irq(uint8_t irqn, uint8_t prio)
{
    volatile uint8_t *nvic_setpri = (volatile uint8_t *)(NVIC_IPR_BASE + irqn);
    *nvic_setpri = prio;
}

/**
 * @brief SYSCLK configuration
 */
void sys_clock_config(void);

/**
 * @brief Enable the FPU
 */
void enable_fpu(void);

/**
 * @brief Enable the MPU
 */
int mpu_enable(void);

#endif
