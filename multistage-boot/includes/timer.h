#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/**
 * @brief Covert Seconds to Milliseconds
 */
#define T_SEC(s) (s * 1000)

/**
 * @brief Covert Minutes to Milliseconds
 */
#define T_MIN(m) (T_SEC((m * 60)))

/**
 * @brief Covert Hours to Milliseconds
 */
#define T_HRS(h) (T_MIN((h * 60)))

/**
 * @brief Enum for timer moode
 *
 * @note ONESHOT triggers only once and stops the timer
 *
 */
typedef enum
{
    TIMER_2 = 2,
    TIMER_3,
    TIMER_4,
    TIMER_5
} timer_no_t;

/**
 * @brief Enum for timer moode
 *
 * @note ONESHOT triggers only once and stops the timer
 *
 */
typedef enum
{
    CONTINOUS,
    ONESHOT,
    OTHERS
} timer_mode_t;

/**
 * @brief Enum for Timer_x Channle
 */
typedef enum
{
    TIMERx_CH1,
    TIMERx_CH2,
    TIMERx_CH3,
    TIMERx_CH4,
} timer_ch_t;

typedef enum
{
    TIMx_CCER_EN,
    TIMx_CCER_P,
    TIMx_CCER_NP,
} timer_ccer_t;

/**
 * @brief Container with timer configuration
 *
 */
typedef struct
{
    timer_no_t timer;
    timer_mode_t mode;
} timer_dt_spec;

/**
 * @brief Callback function definition
 */
typedef void (*timer_callback_t)(void);

/**
 * @brief Initialize timer
 *
 * @param timer_spec - Container for timer configuration
 * @param interval_ms - Interval for timer
 *
 * @return <0 if error otherwise 0
 *
 * @note
 * - TIMER_2 & TIMER_5 can handle a maximum duration of 24Hrs, consider using TIMER_3 and TIMER_4 for durations from 30secs and under.
 * @note
 * - If interval_ms is set to zero, you must use timer_set_arr(...) and timer_set_prescaler(...) to set the trigger dutation or freqency for the timer
 * @note
 * - If pwm_int(..) is used in the code, then TIMER_4 becomes unavailable because it is used as an output compare/PWM mode.
 */
int timer_init(const timer_dt_spec *timer_spec, uint32_t interval_ms);

/**
 * @brief Determines if the counter of the timer is buffered or not
 *
 * @note
 * - If used, the timer counter loads a new value from the ARR only after a update event (UEV)
 */
void timer_set_apre(const timer_dt_spec *timer_spec);

/**
 * @brief Register a new callback for the timer
 *
 * @param timer_spec - Container for timer configuration
 * @param callback - Callback function to be called in the ISR
 *
 */
void timer_register_callback(const timer_dt_spec *timer_spec, timer_callback_t callback);

/**
 * @brief Start the Timer
 *
 * @param timer_spec - Container for timer configuration
 *
 * @note Consider calling timer_register_callback(&timer_spec, callback) for Oneshot mode timers
 * before starting the timer, especially if the duration is short.
 *
 */
void timer_start(const timer_dt_spec *timer_spec);

/**
 * @brief Stop the Timer
 *
 * @param timer_spec - Container for timer configuration
 */
void timer_stop(const timer_dt_spec *timer_spec);

/**
 * @brief Deregister any callback
 *
 * @param timer_spec - Container for timer configuration
 *
 */
void timer_unregister_callback(const timer_dt_spec *timer_spec);

/**
 * @brief Change the interval of the timer
 *
 * @param timer_spec - Container for timer configuration
 * @param interval_ms - New Interval for ttimer
 *
 * @note This is meant for a timer running in continous mode.
 */
int timer_duration_change(const timer_dt_spec *timer_spec, uint32_t interval_ms);

/**
 * @brief Get the counter of the timer. This returns the timer in milliseconds
 * and can be used for measuring short (30s Max) or long (24Hrs Max) durations.
 *
 * @note
 * - This should be used when timer_init(&timer_spec, xx_ms > 0) is used, otherwise
 * use timer_get_counter_raw(..) to get the timer tick in the resolution set
 * @note
 * - Consider using millis() provider by in systick.h for much longer duration
 *
 * @param timer_spec - Container for timer configuration
 * @param interval_ms - New Interval for ttimer
 *
 */
unsigned int timer_get_counter(const timer_dt_spec *timer_spec);

/******************************************Advanced Usage*******************************************************/
unsigned int timer_get_arr(const timer_dt_spec *timer_spec);

unsigned int timer_get_counter_raw(const timer_dt_spec *timer_spec);

int timer_set_arr(const timer_dt_spec *timer_spec, uint32_t arr);

void timer_set_prescaler(const timer_dt_spec *timer_spec, uint16_t prescaler);

void timer_generate_uev(const timer_dt_spec *timer_spec);

void timer_clear_update_flag(const timer_dt_spec *timer_spec);

void timer_ccer_enable(const timer_dt_spec *timer_spec, timer_ch_t timerx_ch, timer_ccer_t ccer_type);

void timer_ccer_disable(const timer_dt_spec *timer_spec, timer_ch_t timerx_ch, timer_ccer_t ccer_type);

void timer_ccrx_set(const timer_dt_spec *timer_spec, timer_ch_t timerx_ch, uint32_t value);

void timer_ccmrx_set(const timer_dt_spec *timer_spec, timer_ch_t timerx_ch, uint8_t value);

void timer_ccmrx_reset(const timer_dt_spec *timer_spec, timer_ch_t timerx_ch);
#endif
