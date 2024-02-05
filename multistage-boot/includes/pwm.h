#ifndef PWM_H_
#define PWM_H

#include "gpios.h"
#include "timer.h"

/**
 * @brief Avalailable PWM channels.
 *
 */
typedef enum
{
    PWM_1, // PD12 - TIM4_CH1
    PWM_2, // PD13 - TIM4_CH2
    PWM_3, // PD14 - TIM4_CH3
    PWM_4  // PD15 - TIM4_CH4

} pwm_no_t;

/**
 * @brief Initialize the PWM
 *
 * @param pwm_no - Enum PWM number
 * @param freq - Frequency of the timer in Hz
 * @param duty_cycle - Duty cycle in percentage 0-100%
 *
 * @return <0 if error otherwise 0
 */
int pwm_init(pwm_no_t pwm_no, uint32_t freq, uint8_t duty_cycle);

/**
 * @brief Start the PWM
 *
 * @param pwm_no - Enum PWM number
 */
void pwm_start(pwm_no_t pwm_no);

/**
 * @brief Change the duty cycle of the PMW
 *
 * @param pwm_no - Enum PWM number
 * @param duty_cycle - Duty cycle in percentage 0-100%
 */
void pwm_change_dutycyle(pwm_no_t pwm_no, uint8_t duty_cycle);

/**
 * @brief Stop the PWM
 *
 * @param pwm_no - Enum PWM number
 */
void pwm_stop(pwm_no_t pwm_no);

#endif
