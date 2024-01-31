#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "gpios.h"
#include "wdt.h"

/*Protos*/
extern int mpu_enable(void);
void sys_init(void);

void main(void)
{
    // Initialize System
    sys_init();

    // Define GPIOS, the Macros are gpios available on the board
    gpio_dt_spec led3 = U_LED3;
    gpio_dt_spec button = U_BUTN;

    /*Init GPIOS*/
    gpio_init(&led3);
    gpio_init(&button);

    // 5secs Start up delay to indicate System Reset
    while (millis() < 5000)
        ;
    /*Set values to the gpios*/
    gpio_set(&led3, GPIO_OUTPUT_HIGH);

    /*System Tick for debaouncing*/
    unsigned int ticks = millis();
    unsigned int debounce = 0;

    /*Setup WDT*/
    int ret = wdt_init(25000);

    while (1)
    {

        // Feed WDT when button is pressed. Button is debounced to remove transient value.
        if (gpio_value(&button) && (millis() - debounce) > 500)
        {
            wdt_feed();
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
