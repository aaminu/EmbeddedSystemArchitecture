#ifndef GPIOS_H_
#define GPIOS_H_

#include <stdint.h>

typedef enum
{
    PORT_A,
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E
} gpio_port_t;

typedef enum
{
    GPIO_INPUT,
    GPIO_OUTPUT,
    GPIO_ALT_FUNC,
    GPIO_ANALOG
} gpio_mode_t;

typedef enum
{
    NONE,
    PULL_UP,
    PULL_DOWN,
} gpio_pupd_t;

typedef enum
{
    PUSH_PULL,
    OPEN_DRAIN
} gpio_otype_t;

typedef struct
{
    gpio_port_t port;
    uint8_t pin_number;
    gpio_mode_t mode;
} gpio_dt_spec;

typedef enum
{
    FALLING_EDGE = 1,
    RISING_EDGE,
    FALLING_OR_RISING_EDGE
} gpio_trigger_t;

typedef void (*gpio_callback_t)(void);

/****BOARD LEDS ABD BUTTON****/
#define U_LED3 ((gpio_dt_spec){PORT_D, 13, GPIO_OUTPUT})
#define U_LED4 ((gpio_dt_spec){PORT_D, 12, GPIO_OUTPUT})
#define U_LED5 ((gpio_dt_spec){PORT_D, 14, GPIO_OUTPUT})
#define U_LED6 ((gpio_dt_spec){PORT_D, 15, GPIO_OUTPUT})
#define U_BUTN ((gpio_dt_spec){PORT_A, 0, GPIO_INPUT})

void gpio_init(gpio_dt_spec *pin_spec);

void gpio_set_pupd(gpio_dt_spec *pin_spec, gpio_pupd_t pupd);

void gpio_set_otype(gpio_dt_spec *pin_spec, gpio_otype_t otype);

void gpio_set(gpio_dt_spec *pin_spec, int value);

void gpio_toggle(gpio_dt_spec *pin_spec);

int gpio_value(gpio_dt_spec *pin_spec);

void gpio_irq_enable(gpio_dt_spec *pin_spec, gpio_trigger_t trigger);

void gpio_register_callback(gpio_dt_spec *pin_spec, gpio_callback_t callback);

void gpio_init_with_irq(gpio_dt_spec *pin_spec, gpio_trigger_t trigger, gpio_callback_t callback);
#endif /*GPIOS_H_*/
