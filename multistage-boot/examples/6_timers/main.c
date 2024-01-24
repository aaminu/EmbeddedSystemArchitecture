#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "gpios.h"
#include "timer.h"

extern int mpu_enable(void);

void sys_init(void);
void toggle_led3_isr(void);
void toggle_led4_isr(void);
void toggle_led5_isr(void);

// Define GPIOS, the Macros are gpios available on the board
gpio_dt_spec led3 = U_LED3;
gpio_dt_spec led4 = U_LED4;
gpio_dt_spec led5 = U_LED5;

void main(void)
{
    // Initialize System
    sys_init();

    // define timers to use
    timer_dt_spec timer_1 = {
        .timer = TIMER_2,
        .mode = ONESHOT,
        .interval_ms = 120000 // Two Minutes
    };

    timer_dt_spec timer_2 = {
        .timer = TIMER_3,
        .mode = CONTINOUS,
        .interval_ms = 500 // 500ms
    };

    timer_dt_spec timer_3 = {
        .timer = TIMER_4,
        .mode = CONTINOUS,
        .interval_ms = 2000 // 2s
    };

    /*Init GPIOS*/
    gpio_init(&led3);
    gpio_init(&led4);
    gpio_init(&led5);

    /*Set values to the gpios*/
    gpio_set(&led3, 0);
    gpio_set(&led4, 0);
    gpio_set(&led4, 0);
    unsigned int ticks = millis();

    timer_init(&timer_1, toggle_led3_isr);
    timer_init(&timer_2, toggle_led4_isr);
    timer_init(&timer_3, toggle_led5_isr);

    while (1)
    {
        WFI();
    }
}

void toggle_led3_isr(void)
{
    gpio_toggle(&led3);
}

void toggle_led4_isr(void)
{
    gpio_toggle(&led4);
}

void toggle_led5_isr(void)
{
    gpio_toggle(&led5);
}

void sys_init(void)
{

    sys_clock_config();
    systick_enable();

#ifdef FPU_USE
    enable_fpu();
#endif

#ifdef MPU_USE
    mpu_enable();
#endif
}
