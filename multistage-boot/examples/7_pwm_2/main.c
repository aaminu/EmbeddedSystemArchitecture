#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "gpios.h"
#include "timer.h"
#include "pwm.h"

void sys_init(void);
void dimmer(void);

void main(void)
{
    // Initialize System
    sys_init();

    const timer_dt_spec timer_3 = {
        .timer = TIMER_3,
        .mode = CONTINOUS,
    };

    // PWM - Channel 4 connected to U_LED5
    pwm_init(PWM_3, 1000, 0);
    pwm_start(PWM_3);

    timer_init(&timer_3, 20);                  // 20ms
    timer_register_callback(&timer_3, dimmer); // Increase/Decrease PWM channel every 20ms
    timer_start(&timer_3);                     // Start all the timers

    while (1)
    {
        WFI()
    }
}

void dimmer(void)
{
    static uint8_t duty_cycle = 0;

    if (duty_cycle > 100)
        duty_cycle = 0;

    pwm_change_dutycyle(PWM_3, duty_cycle);
    duty_cycle++;
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
