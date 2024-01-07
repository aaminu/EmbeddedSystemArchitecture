#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "gpios.h"

static int bss_variable;
static int initialized_variable = 31;

extern int mpu_enable(void);
void sys_init(void);
void toggle_led5(void);

// Define GPIOS, the Macros are gpios available on the board
gpio_dt_spec led3 = U_LED3;
gpio_dt_spec led4 = U_LED4;
gpio_dt_spec button = U_BUTN;

void main(void)
{
    // Initialize System
    sys_init();

    /*Init GPIOS*/
    gpio_init(&led3);
    gpio_init(&led4);
    gpio_init(&button);

    /*Set values to the gpios*/
    gpio_set(&led3, 1);
    gpio_set(&led4, 0);

    /*System Tick for debaouncing*/
    unsigned int ticks = millis();
    unsigned int debounce = 0;

    while (1)
    {
        bss_variable++;
        initialized_variable++;

        // Blink LD3 on the board atleast after a second
        if ((millis() - ticks) / 1000 >= 1)
        {
            gpio_toggle(&led3);
            ticks = millis();
        }

        // Only toggle the LD4  when button is pressed. Button is debounced to remove transient value.
        if (gpio_value(&button) && (millis() - debounce) > 500)
        {
            gpio_toggle(&led4);
            debounce = millis();
        }
    }
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
