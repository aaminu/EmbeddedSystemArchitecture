#include "gpios.h"

/*******RCC*******/
#define RCC_BASE (0x40023800)
#define RCC_AHB1RSTR (*(volatile uint32_t *)(RCC_BASE + 0x10))
#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x30))

/******PORTS_ENABLE*******/
#define GPIOAEN (1 << 0)
#define GPIOBEN (1 << 1)
#define GPIOCEN (1 << 2)
#define GPIODEN (1 << 3)
#define GPIOEEN (1 << 4)

/*******GPIOS BASE**********/
#define GPIOA_BASE (0x40020000)
#define GPIOB_BASE (0x40020400)
#define GPIOC_BASE (0x40020800)
#define GPIOD_BASE (0x40020C00)
#define GPIOE_BASE (0x40021000)

#define SELECT_PORT(port)                                          \
    ((port == PORT_A) ? GPIOA_BASE : (port == PORT_B) ? GPIOB_BASE \
                                 : (port == PORT_C)   ? GPIOC_BASE \
                                 : (port == PORT_D)   ? GPIOD_BASE \
                                 : (port == PORT_E)   ? GPIOE_BASE \
                                                      : 0)

#define ENABLE_BIT(port)                                     \
    ((port == PORT_A) ? GPIOAEN : (port == PORT_B) ? GPIOBEN \
                              : (port == PORT_C)   ? GPIOCEN \
                              : (port == PORT_D)   ? GPIODEN \
                              : (port == PORT_E)   ? GPIOEEN \
                                                   : 0)

/****GPIOS REGS OFFSETS*****/
#define MODER (0x00)
#define OTYPER (0x04)
#define OSPEEDR (0x08)
#define PUPDR (0x0C)
#define IDR (0x10)
#define ODR (0x14)
#define BSRR (0x18)
#define AFRL (0x20)
#define AFRH (0x24)

static uint32_t port_b(gpio_dt_spec *pin_spec)
{
    if (pin_spec->pin_number > 15)
        return 0;

    uint32_t port_base = (uint32_t)SELECT_PORT(pin_spec->port);
    if (!port_base)
        return 0;

    return port_base;
}

void gpio_init(gpio_dt_spec *pin_spec)
{
    uint32_t port_base = port_b(pin_spec);
    if (!port_base)
        return;

    // Enable Clock for port
    RCC_AHB1ENR |= ENABLE_BIT(pin_spec->port);

    volatile uint32_t mode_reg = (*(volatile uint32_t *)(port_base + MODER));
    // clear Mode and set mode bit
    mode_reg &= ~(0x03 << (pin_spec->pin_number * 2));
    mode_reg |= ((uint8_t)pin_spec->mode << (pin_spec->pin_number * 2));

    // update Reg
    *(volatile uint32_t *)(port_base + MODER) |= mode_reg;
}

void gpio_set_pupd(gpio_dt_spec *pin_spec, gpio_pupd_t pupd)
{
    uint32_t port_base = port_b(pin_spec);
    if (!port_base)
        return;

    volatile uint32_t pupd_reg = (*(volatile uint32_t *)(port_base + PUPDR));
    pupd_reg &= ~(0x03 << (pin_spec->pin_number * 2));
    pupd_reg |= ((uint8_t)pupd << (pin_spec->pin_number * 2));

    *(volatile uint32_t *)(port_base + PUPDR) |= pupd_reg;
}

void gpio_set_otype(gpio_dt_spec *pin_spec, gpio_otype_t otype)
{
    if (pin_spec->mode != GPIO_OUTPUT)
        return;

    uint32_t port_base = port_b(pin_spec);
    if (!port_base)
        return;

    volatile uint32_t otype_reg = (*(volatile uint32_t *)(port_base + OTYPER));
    otype_reg &= ~(0x03 << pin_spec->pin_number);
    otype_reg |= ((uint8_t)otype << pin_spec->pin_number);

    *(volatile uint32_t *)(port_base + OTYPER) |= otype_reg;
}

void gpio_set(gpio_dt_spec *pin_spec, int value)
{
    if (pin_spec->mode != GPIO_OUTPUT)
        return;

    uint32_t port_base = port_b(pin_spec);
    if (!port_base)
        return;

    volatile uint32_t bsr_reg = (*(volatile uint32_t *)(port_base + BSRR));

    if (value < 1)
    {
        bsr_reg |= 1 << (pin_spec->pin_number + 16);
        *(volatile uint32_t *)(port_base + BSRR) |= bsr_reg;
        return;
    }

    bsr_reg |= 1 << (pin_spec->pin_number);
    *(volatile uint32_t *)(port_base + BSRR) |= bsr_reg;
}

void gpio_toggle(gpio_dt_spec *pin_spec)
{
    if (pin_spec->mode != GPIO_OUTPUT)
        return;

    uint32_t port_base = port_b(pin_spec);
    if (!port_base)
        return;

    volatile uint32_t odr_reg = (*(volatile uint32_t *)(port_base + ODR));
    if ((odr_reg & (1 << pin_spec->pin_number)))
        gpio_set(pin_spec, 0);
    else
        gpio_set(pin_spec, 1);
}

int gpio_value(gpio_dt_spec *pin_spec)
{
    if (pin_spec->mode != GPIO_INPUT)
        return -1;

    uint32_t port_base = port_b(pin_spec);
    if (!port_base)
        return -1;

    volatile uint32_t idr_reg = (*(volatile uint32_t *)(port_base + IDR));

    return (idr_reg & (1 << pin_spec->pin_number)) >> pin_spec->pin_number;
}
