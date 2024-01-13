#include "wdt.h"

/******WDT*******/
#define IWDG_BASE (0x40003000)
#define IWDG_KR (*(volatile uint32_t *)(IWDG_BASE + 0x00))
#define IWDG_PR (*(volatile uint32_t *)(IWDG_BASE + 0x04))
#define IWDG_RLR (*(volatile uint32_t *)(IWDG_BASE + 0x08))
#define IWDG_SR (*(volatile uint32_t *)(IWDG_BASE + 0x0C))

/******WDT REG VALUES*******/
#define IWDG_KR_RELOAD (0x0000AAAA)
#define IWDG_KR_ACCESS (0x00005555)
#define IWDG_KR_START (0x0000CCCC)
#define IWDG_SR_PVU (1 << 0)
#define IWDG_SR_RVU (1 << 1)

#define LSI_FREQ (0x00007D00)
#define MAX_TIMEOUT (LSI_FREQ)
#define MIN_TIMEOUT (1)
#define PRESCALER_DEF_SHIFT (3)

/*************/
int wdt_init(uint32_t ms)
{
    // Check for WDT limit and guard against 0 as ms
    if ((ms > MAX_TIMEOUT) || (ms < MIN_TIMEOUT))
        return -1;

    uint32_t prescaler = 0;
    // The default value of PR ius zero and LSI is scale by /4 to give approx 8KHz
    // Shift interval in ms to 8kHz by multiplying by 8 or rsh by 3
    uint32_t reload_value = (ms << 3);

    while ((prescaler < 6) && (reload_value > 0xFFF))
    {
        reload_value >>= 1;
        prescaler++;
    }

    // Start WDT
    IWDG_KR = IWDG_KR_START;

    // Unlock KR
    IWDG_KR = IWDG_KR_ACCESS;

    // Set the Prescaler
    while (IWDG_SR & IWDG_SR_PVU)
        ;
    IWDG_PR = prescaler;

    // Set the Reload value
    while (IWDG_SR & IWDG_SR_RVU)
        ;
    IWDG_RLR = reload_value;

    // reload the WDT
    IWDG_KR = IWDG_KR_RELOAD;

    return 0;
}

void wdt_feed(void)
{
    IWDG_KR = IWDG_KR_RELOAD;
}
