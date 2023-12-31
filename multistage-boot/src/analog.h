#ifndef ANALOG_H_
#define ANALOG_H_

#include "gpios.h"

/*ADC RESOLUTION*/

void adc_init(gpio_dt_spec *pin_spec);
int adc_read(gpio_dt_spec *pin_spec);

#endif /*ANALOG_H_*/
