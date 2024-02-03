#ifndef PWM_H_
#define PWM_H

#include "gpios.h"
#include "timer.h"

typedef enum
{
    PWM_1, // PD12 - TIM4_CH1
    PWM_2, // PD13 - TIM4_CH2
    PWM_3, // PD14 - TIM4_CH3
    PWM_4  // PD15 - TIM4_CH4

} pwm_no_t;

int pwm_init(pwm_no_t pwm_no, uint32_t freq, uint8_t duty_cycle);

void pwm_start(pwm_no_t pwm_no);

void pwm_change_dutycyle(pwm_no_t pwm_no, uint8_t duty_cycle);

void pwm_stop(pwm_no_t pwm_no);

#endif
