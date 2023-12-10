#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "gpios.h"

static int bss_variable;
static int initialized_variable = 31;

// extern int mpu_enable(void);

// GPIOS
gpio_dt_spec led = U_LED3;
gpio_dt_spec led1 = U_LED4;
gpio_dt_spec led2 = U_LED5;
gpio_dt_spec button = U_BUTN;

void toggle_led5(void);

void main(void)
{
    // mpu_enable();
    sys_clock_config();
    systick_enable();

    /*Setup GPIOS*/
    gpio_dt_spec input = {PORT_C, 11, GPIO_INPUT};

    gpio_init(&led);
    gpio_init(&led1);
    gpio_init(&led2);
    gpio_init(&button);
    gpio_init_with_irq(&input, RISING_EDGE, toggle_led5);

    gpio_set(&led, 1);
    gpio_set(&led1, 0);
    gpio_set(&led1, 0);

    unsigned int ticks = millis();
    unsigned int debounce = 0;

    while (1)
    {
        // char *c = (char *)malloc(10);
        bss_variable++;
        initialized_variable++;

        // free(c);

        if ((millis() - ticks) / 1000 >= 1)
        {
            gpio_toggle(&led);
            ticks = millis();
        }

        if (gpio_value(&button) && (millis() - debounce) > 500)
        {
            gpio_toggle(&led1);
            debounce = millis();
        }
    }
}

void toggle_led5(void)
{
    gpio_toggle(&led2);
}
