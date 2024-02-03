#include "system.h"
#include "pwm.h"

#define PWM1_MODE (0b01101000)
#define PWM2_MODE (0b01111000)

int pwm_init(pwm_no_t pwm_no, uint32_t freq, uint8_t duty_cycle)
{

    if (duty_cycle > 100)
        return -1;

    gpio_dt_spec pin_spec = {
        .port = PORT_D,
        .mode = GPIO_ALT_FUNC,
    };

    switch (pwm_no)
    {
    case PWM_1:
        pin_spec.pin_number = 12;
        break;
    case PWM_2:
        pin_spec.pin_number = 13;
        break;
    case PWM_3:
        pin_spec.pin_number = 14;
        break;
    case PWM_4:
        pin_spec.pin_number = 15;
        break;
    default:
        return -1;
    };

    // Initialize Pin
    gpio_init(&pin_spec);
    // set Speed
    gpio_set_ospeed(&pin_spec, VERY_HIGH_SPEED);
    // set Alterate function
    gpio_set_altfunc(&pin_spec, AF2);

    // Intialize timer 4
    timer_dt_spec timer_spec = {
        .timer = TIMER_4,
        .mode = OTHERS,
    };
    timer_init(&timer_spec, 0);
    // Disable CCxE in CCER so we can set values in
    timer_ccer_disable(&timer_spec, (timer_ch_t)pwm_no, TIMx_CCER_EN);
    // Clear PSC
    timer_set_prescaler(&timer_spec, 0);

    // Calculate the ARR and PSC based on
    uint32_t reload_value = (APB1_FREQ * 2) / freq;
    uint16_t prescaler = 1;

    // Integer Calc incase reload_value > 65535
    while ((reload_value > (uint16_t)-1) && (prescaler < (uint16_t)-1))
    {
        // CK_CNT (Timerclock) = Fck_psc / (PSC + 1)
        prescaler++;
        reload_value = ((APB1_FREQ * 2) / prescaler) / freq;
    }

    if (prescaler > (uint16_t)-1)
        return -1;

    timer_set_arr(&timer_spec, reload_value);
    timer_set_prescaler(&timer_spec, prescaler);

    // Calc Duty duty cycle value and set it in the CCRx timer Compare/capture channel
    uint32_t duty_value = (reload_value * duty_cycle) / 100;
    timer_ccrx_set(&timer_spec, (timer_ch_t)pwm_no, duty_value);

    // Reset the CCRMx Reg corresponding to the Channel
    timer_ccmrx_reset(&timer_spec, (timer_ch_t)pwm_no);

    // Set the required value to enable PWM1 in CCRMx Reg
    timer_ccmrx_set(&timer_spec, (timer_ch_t)pwm_no, 0x68);

    // Enable CCxE in CCER
    timer_ccer_enable(&timer_spec, (timer_ch_t)pwm_no, TIMx_CCER_EN);

    // Set the Preload APRE in CR1 reg
    timer_set_apre(&timer_spec);

    // Generate Update Event
    timer_generate_uev(&timer_spec);
    // Clear Update Flag
    timer_clear_update_flag(&timer_spec);

    return 0;
}

void pwm_start(pwm_no_t pwm_no)
{
    // Intialize timer 4
    timer_dt_spec timer_spec = {
        .timer = TIMER_4,
        .mode = OTHERS,
    };

    timer_start(&timer_spec);
}

void pwm_change_dutycyle(pwm_no_t pwm_no, uint8_t duty_cycle)
{
    if (duty_cycle > 100)
        return;

    // Intialize timer 4
    timer_dt_spec timer_spec = {
        .timer = TIMER_4,
        .mode = OTHERS,
    };

    uint32_t current_arr = timer_get_arr(&timer_spec);
    uint32_t duty_value = (current_arr * duty_cycle) / 100;
    timer_ccrx_set(&timer_spec, (timer_ch_t)pwm_no, duty_value);
}

void pwm_stop(pwm_no_t pwm_no)
{
    // Intialize timer 4
    timer_dt_spec timer_spec = {
        .timer = TIMER_4,
        .mode = OTHERS,
    };
    timer_stop(&timer_spec);
}
