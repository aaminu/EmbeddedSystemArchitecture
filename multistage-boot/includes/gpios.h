#ifndef GPIOS_H_
#define GPIOS_H_

#include <stdint.h>

typedef enum
{
    PORT_A,
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E,
    /*Add new ports above*/
    PORT_COUNT
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

typedef enum
{
    LOW_SPEED,
    MEDIUM_SPEED,
    HIGH_SPEED,
    VERY_HIGH_SPEED
} gpio_ospeed_t;

typedef enum
{
    FALLING_EDGE = 1,
    RISING_EDGE,
    FALLING_OR_RISING_EDGE
} gpio_trigger_t;

typedef enum
{
    AF0,  /**(system)*/
    AF1,  /** (TIM1/TIM2)*/
    AF2,  /** (TIM3..5)*/
    AF3,  /** (TIM8..11)*/
    AF4,  /** (I2C1..3)*/
    AF5,  /** (SPI1/SPI2)*/
    AF6,  /** (SPI3)*/
    AF7,  /** (USART1..3)*/
    AF8,  /** (USART4..6)*/
    AF9,  /** (CAN1/CAN2, TIM12..14)*/
    AF10, /** (OTG_FS, OTG_HS)*/
    AF11, /** (ETH)*/
    AF12, /** (FSMC, SDIO, OTG_HS(1))*/
    AF13, /** (DCMI)*/
    AF14,
    AF15, /**(EVENTOUT)*/
} gpio_altfunct_t;

typedef enum
{
    GPIO_OUTPUT_LOW,
    GPIO_OUTPUT_HIGH,

} gpio_level_t;

typedef struct
{
    gpio_port_t port;
    uint8_t pin_number;
    gpio_mode_t mode;
} gpio_dt_spec;

typedef void (*gpio_callback_t)(void);

/****BOARD LEDS ABD BUTTON****/
#define U_LED3 ((gpio_dt_spec){PORT_D, 13, GPIO_OUTPUT})
#define U_LED4 ((gpio_dt_spec){PORT_D, 12, GPIO_OUTPUT})
#define U_LED5 ((gpio_dt_spec){PORT_D, 14, GPIO_OUTPUT})
#define U_LED6 ((gpio_dt_spec){PORT_D, 15, GPIO_OUTPUT})
#define U_BUTN ((gpio_dt_spec){PORT_A, 0, GPIO_INPUT})

/**
 * @brief Initialize the GPIO
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 */
void gpio_init(gpio_dt_spec *pin_spec);

/**
 * @brief Set the internal pull up/down resistor for the pin
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 * @param pupd enum gpio_pupd_t (NONE, PULL_UP, PULL_DOWN)
 */
void gpio_set_pupd(gpio_dt_spec *pin_spec, gpio_pupd_t pupd);

/**
 * @brief Set the speed for the output pin,
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 * @param speed enum gpio_ospeed_t type (LOW_SPEED, MEDIUM_SPEED, HIGH_SPEED, VERY_HIGH_SPEED)
 */
void gpio_set_ospeed(gpio_dt_spec *pin_spec, gpio_ospeed_t speed);

/**
 * @brief Set the driver for the output pin, default is PUSH_PULL
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 * @param otype enum gpio_otype_t (PUSH_PULL, OPEN_DRAIN)
 */
void gpio_set_otype(gpio_dt_spec *pin_spec, gpio_otype_t otype);

/**
 * @brief Set the alternate function of the pin, the mode should also be GPIO_ALT_FUNC
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 * @param altfun enum gpio_altfunct_t alternate function
 *
 * @note Using the alt function requires you knowing what you are doing. Please refer to the altfunction
 * table of the board.
 */
void gpio_set_altfunc(gpio_dt_spec *pin_spec, gpio_altfunct_t altfun);

/**
 * @brief Set the state of the PIN
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 * @param pin_level Switch on or off the pin.
 */
void gpio_set(gpio_dt_spec *pin_spec, gpio_level_t pin_level);

/**
 * @brief Toggle the state of an output pin
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 *
 */
void gpio_toggle(gpio_dt_spec *pin_spec);

/**
 * @brief get the state of an input pin
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 *
 */
int gpio_value(gpio_dt_spec *pin_spec);

/**
 * @brief enable interupt handler for an input pin
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 * @param trigger enum gpio_trigger_t type (FALLING_EDGE, RISING_EDGE, FALLING_OR_RISING_EDGE)
 *
 */
void gpio_irq_enable(gpio_dt_spec *pin_spec, gpio_trigger_t trigger);

/**
 * @brief register a callback for the GPIO input pin
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 * @param callback gpio_callback_t function type to be used by the NVIC
 *
 */
void gpio_register_callback(gpio_dt_spec *pin_spec, gpio_callback_t callback);

/**
 * @brief Initialize a GPIO Pin with trigger type and callback function
 * @param pin_spec gpio_dt_spec pointer to gpio config struct containing
 * port name, pin number and mode.
 *  @param trigger enum gpio_trigger_t type (FALLING_EDGE, RISING_EDGE, FALLING_OR_RISING_EDGE)
 * @param callback gpio_callback_t function type to be used by the NVIC
 *
 */
void gpio_init_with_irq(gpio_dt_spec *pin_spec, gpio_trigger_t trigger, gpio_callback_t callback);
#endif /*GPIOS_H_*/
