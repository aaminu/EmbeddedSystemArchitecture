#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "gpios.h"

static int bss_variable;
static int initialized_variable = 31;

// extern int mpu_enable(void);

void main(void)
{
    // mpu_enable();
    sys_clock_config();
    systick_enable();

    /*Setup GPIOS*/
    gpio_dt_spec led = U_LED3;
    gpio_dt_spec led1 = U_LED4;
    gpio_dt_spec button = U_BUTN;

    gpio_init(&led);
    gpio_init(&led1);
    gpio_init(&button);

    gpio_set(&led, 1);
    gpio_set(&led1, 0);

    unsigned int ticks = millis();

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

        if (gpio_value(&button))
            gpio_toggle(&led1);
    }
}
