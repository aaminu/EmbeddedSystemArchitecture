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
void toggle_led5(void);

// Define GPIOS, the Macros are gpios available on the board
gpio_dt_spec led3 = U_LED3;
gpio_dt_spec led4 = U_LED4;
gpio_dt_spec led5 = U_LED5;

void main(void)
{
    // Initialize System
    sys_init();

    // define timers to use
    const timer_dt_spec timer_1 = {
        .timer = TIMER_2,
        .mode = ONESHOT,
    };

    const timer_dt_spec timer_2 = {
        .timer = TIMER_3,
        .mode = CONTINOUS,
    };

    const timer_dt_spec timer_3 = {
        .timer = TIMER_4,
        .mode = CONTINOUS,
    };

    /*Init GPIOS*/
    gpio_init(&led3);
    gpio_init(&led4);
    gpio_init(&led5);

    /*Set values to the gpios*/
    gpio_set(&led3, 1);
    gpio_set(&led4, 0);
    gpio_set(&led4, 0);

    timer_init(&timer_1, T_MIN(3));  // 3 Minutes
    timer_init(&timer_2, 500);       // 500ms
    timer_init(&timer_3, T_SEC(30)); // 30s (Max)

    /*Register callbacks*/
    timer_register_callback(&timer_1, toggle_led3_isr); // Turn of led3 after 3 mins aka OneShot
    timer_register_callback(&timer_2, toggle_led4_isr); // Blink led4 every 500ms (1/2 sec)

    /* Start all the timers*/
    timer_start(&timer_1);
    timer_start(&timer_2);
    timer_start(&timer_3);

    unsigned int time_val = timer_get_counter(&timer_3);
    while (1)
    {
        // Toggle led5 every one second using timer as a clock
        if ((timer_get_counter(&timer_3) - time_val) > 1000)
        {
            toggle_led5();
            time_val = timer_get_counter(&timer_3);
        }

        // Induce a blocking delay for 50ms.
        delay(50);
        // WFI();
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

void toggle_led5(void)
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
