#ifndef ANALOG_H_
#define ANALOG_H_

#include "gpios.h"

/*ADC RESOLUTION*/

/**
 * @brief Enables the ADC and selects the required channel
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 */
void adc_init(gpio_dt_spec *pin_spec);

/**
 * @brief Reads the volatage level on the pin and returns
 * the digital value for it
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 *
 * @retval 0 <= x < 4096 for 0 - 3.3v
 * @retval < 0 for Error
 */
int adc_read(gpio_dt_spec *pin_spec);

/**
 * @brief Reads and returns the volatage level on the pin
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 *
 * @retval 0 <= x <= 3.30v
 * @retval < 0 for Error
 */
double adc_read_volt(gpio_dt_spec *pin_spec);

#endif /*ANALOG_H_*/
