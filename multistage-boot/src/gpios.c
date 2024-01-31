#include "gpios.h"
#include "system.h"

/*******RCC*******/
#define RCC_AHB1RSTR (*(volatile uint32_t *)(RCC_BASE + 0x10))
#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x44))

/******PORTS_ENABLE*******/
#define GPIOAEN (1 << 0)
#define GPIOBEN (1 << 1)
#define GPIOCEN (1 << 2)
#define GPIODEN (1 << 3)
#define GPIOEEN (1 << 4)

#define ENABLE_PORT(port)                                    \
    ((port == PORT_A) ? GPIOAEN : (port == PORT_B) ? GPIOBEN \
                              : (port == PORT_C)   ? GPIOCEN \
                              : (port == PORT_D)   ? GPIODEN \
                              : (port == PORT_E)   ? GPIOEEN \
                                                   : 0)

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

/****EXTI NVIC IRQN****/
#define EXTI0_IRQN (6)
#define EXTI1_IRQN (7)
#define EXTI2_IRQN (8)
#define EXTI3_IRQN (9)
#define EXTI4_IRQN (10)
#define EXTI9_5_IRQN (23)
#define EXTI15_10_IRQN (40)

#define SELECT_EXTI_IRQN(pin_number)                                                            \
    ((pin_number == 0) ? EXTI0_IRQN : (pin_number == 1)                        ? EXTI1_IRQN     \
                                  : (pin_number == 2)                          ? EXTI2_IRQN     \
                                  : (pin_number == 3)                          ? EXTI3_IRQN     \
                                  : (pin_number == 4)                          ? EXTI4_IRQN     \
                                  : ((pin_number >= 5) && (pin_number <= 9))   ? EXTI9_5_IRQN   \
                                  : ((pin_number >= 10) && (pin_number <= 15)) ? EXTI15_10_IRQN \
                                                                               : 0)

/****EXTI REGISTERS*****/
#define EXTI_BASE (0x40013C00)
#define EXTI_IMR (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_EMR (*(volatile uint32_t *)(EXTI_BASE + 0x04))
#define EXTI_RTSR (*(volatile uint32_t *)(EXTI_BASE + 0x08))
#define EXTI_FTSR (*(volatile uint32_t *)(EXTI_BASE + 0x0C))
#define EXTI_SWIER (*(volatile uint32_t *)(EXTI_BASE + 0x10))
#define EXTI_PR (*(volatile uint32_t *)(EXTI_BASE + 0x14))

/****EXTI CONTROL REGISTERS*****/
#define SYSCFGEN (1 << 14)
#define SYSCFG_BASE (0x40013800)
#define SYSCFG_EXTICR1 (*(volatile uint32_t *)(SYSCFG_BASE + 0x08))
#define SYSCFG_EXTICR2 (*(volatile uint32_t *)(SYSCFG_BASE + 0x0C))
#define SYSCFG_EXTICR3 (*(volatile uint32_t *)(SYSCFG_BASE + 0x010))
#define SYSCFG_EXTICR4 (*(volatile uint32_t *)(SYSCFG_BASE + 0x14))

/*****EXTI CR VALUES *******/
#define PA_x (0)
#define PB_x (1)
#define PC_x (2)
#define PD_x (3)
#define PE_x (4)

#define EXTI_CR_PORT_GROUP(port)                       \
    ((port == PORT_A) ? PA_x : (port == PORT_B) ? PB_x \
                           : (port == PORT_C)   ? PC_x \
                           : (port == PORT_D)   ? PD_x \
                           : (port == PORT_E)   ? PE_x \
                                                : 15)

/******Static. Don't modify********/
static void empty_callback(void) {} // Space holder

static void (*volatile callback_array[16])(void) = {
    empty_callback, // EXTI0 callback
    empty_callback, // EXTI1 callback
    empty_callback, // EXTI2 callback
    empty_callback, // EXTI3 callback
    empty_callback, // EXTI4 callback
    empty_callback, // EXTI5 callback
    empty_callback, // EXTI6 callback
    empty_callback, // EXTI7 callback
    empty_callback, // EXTI8 callback
    empty_callback, // EXTI9 callback
    empty_callback, // EXTI10 callback
    empty_callback, // EXTI11 callback
    empty_callback, // EXTI12 callback
    empty_callback, // EXTI13 callback
    empty_callback, // EXTI14 callback
    empty_callback, // EXTI15 callback
};

static uint32_t port_base_selector(gpio_dt_spec *pin_spec)
{
    if (pin_spec->pin_number > 15)
        return 0;

    uint32_t port_base = (uint32_t)SELECT_PORT(pin_spec->port);

    return port_base;
}
/****************************************/

void gpio_init(gpio_dt_spec *pin_spec)
{
    uint32_t port_base = port_base_selector(pin_spec);
    if (!port_base)
        return;

    // Enable Clock for port
    RCC_AHB1ENR |= ENABLE_PORT(pin_spec->port);

    volatile uint32_t mode_reg = (*(volatile uint32_t *)(port_base + MODER));
    // clear Mode and set mode bit
    mode_reg &= ~(0x03 << (pin_spec->pin_number * 2));
    mode_reg |= ((uint8_t)pin_spec->mode << (pin_spec->pin_number * 2));

    // update Reg
    *(volatile uint32_t *)(port_base + MODER) = mode_reg;
}

void gpio_set_pupd(gpio_dt_spec *pin_spec, gpio_pupd_t pupd)
{
    uint32_t port_base = port_base_selector(pin_spec);
    if (!port_base)
        return;

    volatile uint32_t pupd_reg = (*(volatile uint32_t *)(port_base + PUPDR));
    pupd_reg &= ~(0x03 << (pin_spec->pin_number * 2));
    pupd_reg |= ((uint8_t)pupd << (pin_spec->pin_number * 2));

    *(volatile uint32_t *)(port_base + PUPDR) = pupd_reg;
}

void gpio_set_otype(gpio_dt_spec *pin_spec, gpio_otype_t otype)
{
    if (pin_spec->mode != GPIO_OUTPUT)
        return;

    uint32_t port_base = port_base_selector(pin_spec);
    if (!port_base)
        return;

    volatile uint32_t otype_reg = (*(volatile uint32_t *)(port_base + OTYPER));
    otype_reg &= ~(0x01 << pin_spec->pin_number);
    otype_reg |= ((uint8_t)otype << pin_spec->pin_number);

    *(volatile uint32_t *)(port_base + OTYPER) = otype_reg;
}

void gpio_set_ospeed(gpio_dt_spec *pin_spec, gpio_ospeed_t speed)
{
    if (pin_spec->mode != GPIO_OUTPUT)
        return;

    uint32_t port_base = port_base_selector(pin_spec);
    if (!port_base)
        return;

    volatile uint32_t ospeed_reg = (*(volatile uint32_t *)(port_base + OSPEEDR));
    ospeed_reg &= ~(0x03 << (pin_spec->pin_number * 2));
    ospeed_reg |= ((uint8_t)speed << (pin_spec->pin_number * 2));

    *(volatile uint32_t *)(port_base + OSPEEDR) = ospeed_reg;
}

void gpio_set_altfunc(gpio_dt_spec *pin_spec, gpio_altfunct_t altfun)
{
    if (pin_spec->mode != GPIO_ALT_FUNC)
        return;

    uint32_t port_base = port_base_selector(pin_spec);
    if (!port_base)
        return;

    if (pin_spec->pin_number < 8)
    {
        volatile uint32_t afrlh_reg = (*(volatile uint32_t *)(port_base + AFRL));
        afrlh_reg &= ~(0x0f << (pin_spec->pin_number * 4));
        afrlh_reg |= ((uint8_t)altfun << (pin_spec->pin_number * 4));

        *(volatile uint32_t *)(port_base + AFRL) = afrlh_reg;
    }
    else
    {
        volatile uint32_t afrlh_reg = (*(volatile uint32_t *)(port_base + AFRH));
        afrlh_reg &= ~(0x0f << ((pin_spec->pin_number - 8) * 4));
        afrlh_reg |= ((uint8_t)altfun << ((pin_spec->pin_number - 8) * 4));

        *(volatile uint32_t *)(port_base + AFRH) = afrlh_reg;
    }
}

void gpio_set(gpio_dt_spec *pin_spec, gpio_level_t pin_level)
{
    if (pin_spec->mode != GPIO_OUTPUT)
        return;

    uint32_t port_base = port_base_selector(pin_spec);
    if (!port_base)
        return;

    if (!(uint8_t)pin_level)
    {
        (*(volatile uint32_t *)(port_base + BSRR)) |= 1 << (pin_spec->pin_number + 16);
        return;
    }

    *(volatile uint32_t *)(port_base + BSRR) |= 1 << (pin_spec->pin_number);
}

void gpio_toggle(gpio_dt_spec *pin_spec)
{
    if (pin_spec->mode != GPIO_OUTPUT)
        return;

    uint32_t port_base = port_base_selector(pin_spec);
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

    uint32_t port_base = port_base_selector(pin_spec);
    if (!port_base)
        return -1;

    volatile uint32_t idr_reg = (*(volatile uint32_t *)(port_base + IDR));

    return (idr_reg & (1 << pin_spec->pin_number)) >> pin_spec->pin_number;
}

void gpio_irq_enable(gpio_dt_spec *pin_spec, gpio_trigger_t trigger)
{
    if (pin_spec->pin_number > 15)
        return;

    if (pin_spec->mode != GPIO_INPUT)
        return;

    // Enable SYSCFG Clock
    RCC_APB2ENR |= SYSCFGEN;

    uint8_t shifter = (pin_spec->pin_number % 4) * 4;
    uint8_t port_group = EXTI_CR_PORT_GROUP(pin_spec->port);

    // Mask/Enable the EXTI_CR
    switch (pin_spec->pin_number)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        SYSCFG_EXTICR1 &= ~(0x0F << shifter);      // clear port group EXTI LINE
        SYSCFG_EXTICR1 |= (port_group << shifter); // select port group on EXTI LINE
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        SYSCFG_EXTICR2 &= ~(0x0F << shifter);      // clear port group EXTI LINE
        SYSCFG_EXTICR2 |= (port_group << shifter); // select port group on EXTI LINE
        break;
    case 8:
    case 9:
    case 10:
    case 11:
        SYSCFG_EXTICR3 &= ~(0x0F << shifter);      // clear port group EXTI LINE
        SYSCFG_EXTICR3 |= (port_group << shifter); // select port group on EXTI LINE
        break;
    case 12:
    case 13:
    case 14:
    case 15:
        SYSCFG_EXTICR4 &= ~(0x0F << shifter);      // clear port group EXTI LINE
        SYSCFG_EXTICR4 |= (port_group << shifter); // select port group on EXTI LINE
        break;
    }

    // Set the trigger
    switch (trigger)
    {
    case RISING_EDGE:
        EXTI_RTSR |= (1 << pin_spec->pin_number);
        break;
    case FALLING_EDGE:
        EXTI_FTSR |= (1 << pin_spec->pin_number);
        break;
    case FALLING_OR_RISING_EDGE:
        EXTI_RTSR |= (1 << pin_spec->pin_number);
        EXTI_FTSR |= (1 << pin_spec->pin_number);
    }

    // Enable IMR
    EXTI_IMR |= (1 << pin_spec->pin_number);

    // Enable IRQN
    uint8_t irqn = SELECT_EXTI_IRQN(pin_spec->pin_number);
    nvic_enable_irq(irqn);
}

void gpio_register_callback(gpio_dt_spec *pin_spec, gpio_callback_t callback)
{
    if (pin_spec->pin_number > 15)
        return;

    // Append the callback to array container
    callback_array[pin_spec->pin_number] = callback;
}

void gpio_init_with_irq(gpio_dt_spec *pin_spec, gpio_trigger_t trigger, gpio_callback_t callback)
{
    gpio_init(pin_spec);
    gpio_irq_enable(pin_spec, trigger);
    gpio_register_callback(pin_spec, callback);
}

/*Interrupt Service Routine for EXTI0 */
void _isr_exti0(void)
{
    EXTI_PR |= (1 << 0);
    callback_array[0]();
}

/*Interrupt Service Routine for EXTI1 */
void _isr_exti1(void)
{
    EXTI_PR |= (1 << 1);
    callback_array[1]();
}

/*Interrupt Service Routine for EXTI2 */
void _isr_exti2(void)
{
    EXTI_PR |= (1 << 2);
    callback_array[2]();
}

/*Interrupt Service Routine for EXTI3*/
void _isr_exti3(void)
{
    EXTI_PR |= (1 << 3);
    callback_array[3]();
}

/*Interrupt Service Routine for EXTI4*/
void _isr_exti4(void)
{
    EXTI_PR |= (1 << 4);
    callback_array[4]();
}

/*Interrupt Service Routine for EXTI9-5*/
void _isr_exti9_5(void)
{
    if (EXTI_PR & (uint32_t)(1 << 5))
    {
        EXTI_PR |= (1 << 5);
        callback_array[5]();
    }

    if (EXTI_PR & (uint32_t)(1 << 6))
    {
        EXTI_PR |= (1 << 6);
        callback_array[6]();
    }

    if (EXTI_PR & (uint32_t)(1 << 7))
    {
        EXTI_PR |= (1 << 7);
        callback_array[7]();
    }

    if (EXTI_PR & (uint32_t)(1 << 8))
    {
        EXTI_PR |= (1 << 8);
        callback_array[8]();
    }

    if (EXTI_PR & (uint32_t)(1 << 9))
    {
        EXTI_PR |= (1 << 9);
        callback_array[9]();
    }
}

/*Interrupt Service Routine for EXTI15-10*/
void _isr_exti15_10(void)
{
    if (EXTI_PR & (uint32_t)(1 << 10))
    {
        EXTI_PR |= (1 << 10);
        callback_array[10]();
    }

    if (EXTI_PR & (uint32_t)(1 << 11))
    {
        EXTI_PR |= (1 << 11);
        callback_array[11]();
    }

    if (EXTI_PR & (uint32_t)(1 << 12))
    {
        EXTI_PR |= (1 << 10);
        callback_array[12]();
    }

    if (EXTI_PR & (uint32_t)(1 << 13))
    {
        EXTI_PR |= (1 << 10);
        callback_array[13]();
    }

    if (EXTI_PR & (uint32_t)(1 << 14))
    {
        EXTI_PR |= (1 << 14);
        callback_array[14]();
    }

    if (EXTI_PR & (uint32_t)(1 << 15))
    {
        EXTI_PR |= (1 << 15);
        callback_array[15]();
    }
}
