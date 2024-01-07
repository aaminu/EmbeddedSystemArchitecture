#ifndef ANALOG_H_
#define ANALOG_H_

#include "gpios.h"

typedef enum
{
    ADC1,
    ADC2,
    ADC3
} adc_controller_t;

typedef enum
{
    ADC_PCLK_DIV2,
    ADC_PCLK_DIV4,
    ADC_PCLK_DIV6,
    ADC_PCLK_DIV8
} adc_clock_prescaler_t;

typedef enum
{
    ADC_SINGLE_MODE,
    ADC_CONTINOUS_MODE,
    ADC_SCAN_MODE,
    ADC_DISCONTINOUS_MODE = 4
} adc_mode_t;

typedef enum
{
    DISABLE,
    ENABLE
} adc_mode_trigger_t;

typedef enum
{
    ADC_SAMPLETIME_3CYCLES,
    ADC_SAMPLETIME_15CYCLES,
    ADC_SAMPLETIME_28CYCLES,
    ADC_SAMPLETIME_56CYCLES,
    ADC_SAMPLETIME_84CYCLES,
    ADC_SAMPLETIME_112CYCLES,
    ADC_SAMPLETIME_144CYCLES,
    ADC_SAMPLETIME_480CYCLES,
} adc_sampletime_t;

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
float adc_read_volt(gpio_dt_spec *pin_spec);

float adc_temperature(void);

#endif /*ANALOG_H_*/
