#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "gpios.h"
#include "analog.h"

extern int mpu_enable(void);
void sys_init(void);

void main(void)
{
    // Initialize System
    sys_init();

    // GPIO
    gpio_dt_spec led3 = U_LED3;
    gpio_init(&led3);
    gpio_set(&led3, 1);
    unsigned int led_ticks = millis();

    // ADC
    gpio_dt_spec adc_pin = {PORT_A, 5, GPIO_ANALOG};
    adc_init(&adc_pin);
    int adc_value = adc_read(&adc_pin);
    float adc_volts = adc_read_volt(&adc_pin);
    unsigned int adc_ticks = millis();

    while (1)
    {
        // Blink atleast every sec
        if ((millis() - led_ticks) / 1000 >= 1)
        {
            gpio_toggle(&led3);
            led_ticks = millis();
        }

        // Measure ADC atleast every 30sec
        if ((millis() - adc_ticks) / 1000 >= 30)
        {
            adc_ticks = millis();
            adc_value = adc_read(&adc_pin);      // Return the Digitial value of the measure voltage
            adc_volts = adc_read_volt(&adc_pin); // returns the voltage level compared in V
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
