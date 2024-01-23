#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

typedef enum
{
    TIMER_2 = 2,
    TIMER_3,
    TIMER_4,
    TIMER_5
} timer_no_t;

typedef enum
{
    CONTINOUS,
    ONESHOT
} timer_mode_t;

typedef struct
{
    timer_no_t timer;
    timer_mode_t mode;
    uint32_t interval_ms;
} timer_dt_spec;

typedef void (*timer_callback_t)(void);

int timer_init(timer_dt_spec *timer_spec, timer_callback_t callback);
void timer_register_callback(timer_dt_spec *timer_spec, timer_callback_t callback);
void timer_unregister_callback(timer_dt_spec *timer_spec);
int timer_duration_change(timer_dt_spec *timer_spec, uint32_t interval_ms);

#endif
