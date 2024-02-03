#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "gpios.h"
#include "timer.h"
#include "pwm.h"

void sys_init(void);
void toggle_led3_isr(void);

// Define GPIOS
gpio_dt_spec led3 = U_LED3;

void main(void)
{
    // Initialize System
    sys_init();

    const timer_dt_spec timer_2 = {
        .timer = TIMER_3,
        .mode = CONTINOUS,
    };

    /*Init GPIOS*/
    gpio_init(&led3);
    /*Set values to the gpios*/
    gpio_set(&led3, GPIO_OUTPUT_HIGH);

    timer_init(&timer_2, 500);                          // 500ms
    timer_register_callback(&timer_2, toggle_led3_isr); // Blink led4 every 500ms (1/2 sec)
    timer_start(&timer_2);                              // Start all the timers

    // PWM - Channel 4 connected to U_LED6
    int duty_cycle = 100;
    pwm_init(PWM_4, 100000, duty_cycle);
    pwm_start(PWM_4);

    while (1)
    {
        // Simulate a blocking breathing LED
        while (duty_cycle > 0)
        {
            delay(10);
            duty_cycle--;
            pwm_change_dutycyle(PWM_4, duty_cycle);
        }
        delay(20);
        while (duty_cycle < 100)
        {
            delay(10);
            duty_cycle++;
            pwm_change_dutycyle(PWM_4, duty_cycle);
        }
    }
}

void toggle_led3_isr(void)
{

    gpio_toggle(&led3);
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
