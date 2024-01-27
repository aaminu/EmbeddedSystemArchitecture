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
    ONESHOT
} timer_mode_t;

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
 * @brief Initialize timer with  a callback of what action to perform
 *
 * @param timer_spec - Container for timer configuration
 * @param interval_ms - Interval for timer
 *
 * @return <0 if error otherwise 0
 *
 * @note TIMER_2 & TIMER_5 can handle a maximum duration of 24Hrs,
 * consider using TIMER_3 and TIMER_4 for durations from 30secs and under
 */
int timer_init(const timer_dt_spec *timer_spec, uint32_t interval_ms);

/**
 * @brief Register a new callback for the timer
 *
 * @param timer_spec - Container for timer configuration
 * @param callback - Callback function to be called in the ISR
 *
 */
void timer_register_callback(const timer_dt_spec *timer_spec, timer_callback_t callback);

/**
 * @brief Initialize timer with  a callback of what action to perform
 *
 * @param timer_spec - Container for timer configuration
 *
 * @note Consider calling timer_register_callback(&timer_spec, callback) for Oneshot mode timers
 * before starting the timer, especially if the duration is short.
 *
 */
void timer_start(const timer_dt_spec *timer_spec);

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
 * @note Consider using millis() provider by in systick.h for much longer duration
 *
 * @param timer_spec - Container for timer configuration
 * @param interval_ms - New Interval for ttimer
 *
 */
unsigned int timer_get_counter(const timer_dt_spec *timer_spec);

#endif
