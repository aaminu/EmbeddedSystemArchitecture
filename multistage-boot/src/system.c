#include "system.h"

/***Flash Access Control Register***/
#define FLASH_ACR_BASE (0x40023C00)
#define FLASH_ACR (*(volatile uint32_t *)(FLASH_ACR_BASE + 0x00))
#define FLASH_ACR_EN_PREFETCH (1 << 8)
#define FLASH_ACR_EN_INST_CACHE (1 << 9)
#define FLASH_ACR_EN_DATA_CACHE (1 << 10)

/*******RCC*******/
#define RCC_BASE (0x40023800)
#define RCC_CR (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_APB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x40))

#define RCC_CR_PLLRDY (1 << 25)
#define RCC_CR_PLLON (1 << 24)
#define RCC_CR_HSERDY (1 << 17)
#define RCC_CR_HSEON (1 << 16)
#define RCC_CR_HSIRDY (1 << 1)
#define RCC_CR_HSION (1 << 0)
#define RCC_PLLCFGR_PLLSRC (1 << 22)
#define RCC_PWR_EN (1 << 28)

#define RCC_CFGR_SW_HSI 0x0
#define RCC_CFGR_SW_HSE 0x1
#define RCC_CFGR_SW_PLL 0x2
#define RCC_CFGR_SWS_HSIRDY (RCC_CFGR_SW_HSI << 2)
#define RCC_CFGR_SWS_HSERDY (RCC_CFGR_SW_HSE << 2)
#define RCC_CFGR_SWS_PLLRDY (RCC_CFGR_SW_PLL << 2)

/*AHB Prescaler*/
#define RCC_PRESCALER_DIV_NONE 0
/*APB Low & High speed prescaler (APB1 & APB2)*/
#define RCC_PRESCALER_AHB_DIV_2 4
#define RCC_PRESCALER_AHB_DIV_4 5
#define RCC_PRESCALER_AHB_DIV_8 6
#define RCC_PRESCALER_AHB_DIV_16 7

#define PLLM 8
#define PLLN 336
#define PLLP 0 // PLLP = 2
#define PLLQ 7
#define PLLR 0

/******PWR*******/
#define PWR_BASE (0x40007000)
#define PWR_CR (*(volatile uint32_t *)(PWR_BASE + 0x00))

#define RCC_CR_VOS_EN (1 << 14)

/*Export NVIC Inline*/
extern inline void nvic_disable_irq(uint8_t n);
extern inline void nvic_enable_irq(uint8_t n);
extern inline void nvic_set_pending_irq(uint8_t n);
extern inline void nvic_clear_pending_irq(uint8_t n);
extern inline void nvic_set_prio_irq(uint8_t n, uint8_t prio);

/**
 * @brief Sets the flash wait state in the ACR region,
 * This is necessary to avoid Hardfault during setting
 * the system clock due to the CPU speed and access
 * time of the Flash esp. for system with XIP
 */
static void set_flash_waitstates(void)
{
    FLASH_ACR |= 5 | FLASH_ACR_EN_PREFETCH | FLASH_ACR_EN_INST_CACHE | FLASH_ACR_EN_DATA_CACHE;
}

/**
 * @brief clock configuration
 */
static void clock_config(void)
{
    volatile uint32_t reg;
    /*Internal high-speed clock (HSI) enable*/
    RCC_CR |= RCC_CR_HSION;
    DMB();
    /*Wait for HSI to become stable*/
    while (!(RCC_CR & RCC_CR_HSIRDY))
        ;

    /*Set HSI as SYSCLK*/
    reg = RCC_CFGR;
    reg &= ~((1 << 1) | (1 << 0)); // Set last two bits to zero
    RCC_CFGR = (reg | RCC_CFGR_SW_HSI);
    DMB();

    /* External high-speed clock (HSE) enable */
    RCC_CR |= RCC_CR_HSEON;
    DMB();
    while (!(RCC_CR & RCC_CR_HSERDY))
        ;

    /*Enable Power Enable Clock*/
    RCC_APB1ENR |= RCC_PWR_EN;
    PWR_CR |= RCC_CR_VOS_EN;
    DMB();

    /* Set prescalers for AHB, APB1(Lowspeed), APB2(High speed)*/
    reg = RCC_CFGR;
    reg &= ~(0xF0);                                 // HPRE: control AHB clock division factor
    RCC_CFGR = (reg | RCC_PRESCALER_DIV_NONE << 4); // AHB
    DMB();
    reg = RCC_CFGR;
    reg &= ~(0x07 << 10);                             // PPRE1:  control APB low-speed clock division factor
    RCC_CFGR = (reg | RCC_PRESCALER_AHB_DIV_4 << 10); // APB1 must not exceed 42MHz
    DMB();
    reg = RCC_CFGR;
    reg &= ~(0x07 << 13);                             // PPRE1:  control APB high-speed clock division factor
    RCC_CFGR = (reg | RCC_PRESCALER_AHB_DIV_2 << 13); // APB2 must not exceed 84MHz
    DMB();

    /*Configure PLL: SRC, M, N, P, Q*/
    reg = RCC_PLLCFGR;
    reg &= ~(PLL_FULL_MASK); // Clear Register to Zero
    RCC_PLLCFGR = reg | RCC_PLLCFGR_PLLSRC | PLLM | (PLLN << 6) | (PLLP << 16) | (PLLQ << 24);
    DMB();

    /*Enable PLL and wait to Stabilize*/
    RCC_CR |= RCC_CR_PLLON;
    DMB();
    while (!(RCC_CR & RCC_CR_PLLRDY))
        ;

    /*Set PLL as SYSCLK */
    reg = RCC_CFGR;
    reg &= ~((1 << 1) | (1 << 0)); // Set last two bits to zero
    RCC_CFGR = (reg | RCC_CFGR_SW_PLL);
    DMB();
    while (!(RCC_CFGR & RCC_CFGR_SWS_PLLRDY))
        ;

    /*Disable HSI*/
    RCC_CR &= ~RCC_CR_HSION;
}

void sys_clock_config(void)
{
    set_flash_waitstates();
    clock_config();
}
