#include <stdint.h>
#include "systick.h"

#define SYST_BASE (0xE000E010)
#define SYST_CSR (*(volatile uint32_t *)(SYST_BASE + 0x00))
#define SYST_RVR (*(volatile uint32_t *)(SYST_BASE + 0x04))
#define SYST_CVR (*(volatile uint32_t *)(SYST_BASE + 0x08))
#define SYST_CALIB (*(volatile uint32_t *)(SYST_BASE + 0x0C))

#define CPU_FREQ (168000000)

#define SYST_CSR_EN (1 << 0)
#define SYST_CSR_DIS (0 << 0)
#define SYST_CSR_TICKINT_EN (1 << 1)
#define SYST_CSR_CLK_EXT (0 << 2)
#define SYST_CSR_TICKINT_PROC (1 << 2)

volatile uint32_t jiffies = 0;

void systick_enable()
{
    SYST_RVR = ((CPU_FREQ / 1000) - 1);
    SYST_CVR = 0;
    SYST_CSR = SYST_CSR_EN | SYST_CSR_TICKINT_EN | SYST_CSR_TICKINT_PROC;
}

void systick_disable()
{
    SYST_RVR = 0;
}

void _systick_isr(void)
{
    jiffies++;
}

unsigned int millis()
{
    return (unsigned int)jiffies;
}
