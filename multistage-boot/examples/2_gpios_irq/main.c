#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "gpios.h"

extern int mpu_enable(void);
void sys_init(void);
void toggle_led5(void);

// GPIOS
gpio_dt_spec led3 = U_LED3;
gpio_dt_spec led5 = U_LED5;
gpio_dt_spec input = {PORT_C, 11, GPIO_INPUT};

void main(void)
{
    // Initialize System
    sys_init();

    // Initialize
    gpio_init(&led3);
    gpio_init(&led5);
    gpio_init_with_irq(&input, RISING_EDGE, toggle_led5);

    /*Set values to the gpios*/
    gpio_set(&led3, 1); // High
    gpio_set(&led5, 0); // Low

    // For non-blocking led-blinking
    unsigned int ticks = millis();

    while (1)
    {

        if ((millis() - ticks) / 1000 >= 1)
        {
            gpio_toggle(&led3);
            ticks = millis();
        }
    }
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
