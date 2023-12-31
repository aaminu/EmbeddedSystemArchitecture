#include "analog.h"
#include "system.h"

/*******RCC*******/
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x44))

/******PORTS_ENABLE*******/
#define ADC1EN (1 << 8)
#define ADC2EN (1 << 9)
#define ADC3EN (1 << 10)

/******ADC*******/
#define ADC1_BASE (0x40012000)
#define ADC2_BASE (0x40012000 + 0x100)
#define ADC3_BASE (0x40012000 + 0x200)
#define ADC_COMMON_REG_BASE (0x40012000 + 0x300)

/******ADC REG OFFSETS********/
#define ADC_SR (0x00)
#define ADC_CR1 (0x04)
#define ADC_CR2 (0x08)
#define ADC_SMPR1 (0x0C)
#define ADC_SMPR2 (0x10)
#define ADC_SQR1 (0x2C)
#define ADC_SQR1 (0x2C)
#define ADC_SQR2 (0x30)
#define ADC_SQR3 (0x34)
#define ADC_DR (0x4C)

#define ADC_CSR (0x00)
#define ADC_CCR (0x04)
#define ADC_CDR (0x08)

/*****FLAGS*****/
#define ADC_SR_EOC (1 << 1)       // Regular channel end of conversion
#define ADC_CR1_SCAN (1 << 8)     // Scan mode enabled
#define ADC_CR2_ADON (1 << 0)     // ADC On
#define ADC_CR2_CONT (1 << 1)     // Continous conversion enabled
#define ADC_CR2_SWSTART (1 << 30) // Start conversion of regular channel

/***ADC Prescalers***/
#define ADC_CCR_ADCPRE_PCLK_DIV2 (0x0 << 16)
#define ADC_CCR_ADCPRE_PCLK_DIV4 (0x1 << 16)
#define ADC_CCR_ADCPRE_PCLK_DIV6 (0x2 << 16)
#define ADC_CCR_ADCPRE_PCLK_DIV8 (0x3 << 16)

/*****SAMPLING TIME*****/
#define ADC_SMPR_3_CYCLE (0x00)
#define ADC_SMPR_15_CYCLE (0x01)
#define ADC_SMPR_28_CYCLE (0x02)
#define ADC_SMPR_56_CYCLE (0x03)
#define ADC_SMPR_84_CYCLE (0x04)
#define ADC_SMPR_112_CYCLE (0x05)
#define ADC_SMPR_144_CYCLE (0x06)
#define ADC_SMPR_480_CYCLE (0x07)

/***DATA ALIGN*********/
#define ADC_CR2_ALIGNR (0 << 11)
#define ADC_CR2_ALIGNL (1 << 11)

/********OTHERS*********/
#define CHANNEL_ERROR (100)

/**************Static Function**************************/

/**
 * @brief Function returns the required channel for ADC usagae. The
 * function is used in the simple adc_init(gpio_dt_spec *pin_spec) and
 *  adc_read(gpio_dt_spec *pin_spec) function.
 * if the port and pin number doesn't match any of the condition,
 * a -CHANNEL_ERROR is returned
 */
static int8_t adc_channel(gpio_dt_spec *pin_spec)
{
    int channel = -CHANNEL_ERROR;

    switch (pin_spec->port)
    {
    case PORT_A:
        if (pin_spec->pin_number <= 7)
        {
            channel = pin_spec->pin_number;
            break;
        }
        channel = -CHANNEL_ERROR;
        break;

    case PORT_B:
        if (pin_spec->pin_number <= 1)
        {
            channel = pin_spec->pin_number + 8;
            break;
        }
        channel = -CHANNEL_ERROR;
        break;

    case PORT_C:
        if (pin_spec->pin_number <= 5)
        {
            channel = pin_spec->pin_number + 10;
            break;
        }
        channel = -CHANNEL_ERROR;
        break;

    default:
        channel = -CHANNEL_ERROR;
        break;
    }

    return channel;
}

/****************************************/
void adc_init(gpio_dt_spec *pin_spec)
{
    // Check if corrent mode
    if (pin_spec->mode != GPIO_ANALOG)
        return;

    // Check if Pin is suitable for ADC
    int channel = adc_channel(pin_spec);
    if (channel < 0)
        return;

    // Enable ADC CLK
    RCC_APB2ENR |= ADC1EN;

    // Initial GPIO as ADC
    gpio_init(pin_spec);

    // Disable ADC
    (*(volatile uint32_t *)(ADC1_BASE + ADC_CR2)) &= ~(ADC_CR2_ADON);

    // Set the Clock Prescaler (clear amd set)
    (*(volatile uint32_t *)(ADC_COMMON_REG_BASE + ADC_CCR)) &= ~(0x03 << 16);
    (*(volatile uint32_t *)(ADC_COMMON_REG_BASE + ADC_CCR)) |= ADC_CCR_ADCPRE_PCLK_DIV2;

    // Disable Scan Mode
    (*(volatile uint32_t *)(ADC1_BASE + ADC_CR1)) &= ~(ADC_CR1_SCAN);

    // Disable Continous Mode
    (*(volatile uint32_t *)(ADC1_BASE + ADC_CR2)) &= ~(ADC_CR2_CONT);

    // Sampling Frequency
    if (channel < 9)
    {
        // clear
        (*(volatile uint32_t *)(ADC1_BASE + ADC_SMPR2)) &= ~(0x07 << (channel * 3));
        (*(volatile uint32_t *)(ADC1_BASE + ADC_SMPR2)) |= (ADC_SMPR_480_CYCLE << (channel * 3));
    }
    else
    {
        // clear
        (*(volatile uint32_t *)(ADC1_BASE + ADC_SMPR1)) &= ~(0x07 << ((channel - 10) * 3));
        (*(volatile uint32_t *)(ADC1_BASE + ADC_SMPR1)) |= (ADC_SMPR_480_CYCLE << ((channel - 10) * 3));
    }

    // Enable ADC
    (*(volatile uint32_t *)(ADC1_BASE + ADC_CR2)) |= ADC_CR2_ADON;
}

int adc_read(gpio_dt_spec *pin_spec)
{
    // Check if Pin is suitable for ADC
    int channel = adc_channel(pin_spec);
    if (channel < 0)
        return -1;

    // Disable ADC
    (*(volatile uint32_t *)(ADC1_BASE + ADC_CR2)) &= ~(ADC_CR2_ADON);

    // Set Channel to SQR_x
    (*(volatile uint32_t *)(ADC1_BASE + ADC_SQR3)) |= channel;

    // Enable ADC
    (*(volatile uint32_t *)(ADC1_BASE + ADC_CR2)) |= ADC_CR2_ADON;

    // Start Conversion
    (*(volatile uint32_t *)(ADC1_BASE + ADC_CR2)) |= ADC_CR2_SWSTART;

    // Wait for Conversion to Start
    while ((*(volatile uint32_t *)(ADC1_BASE + ADC_CR2)) & ADC_CR2_SWSTART)
    {
        ;
    }

    // Wait for End of Conversion
    while (((*(volatile uint32_t *)(ADC1_BASE + ADC_SR)) & ADC_SR_EOC) == 0)
    {
        ;
    }

    return (int)(*(volatile uint32_t *)(ADC1_BASE + ADC_DR));
}

double adc_read_volt(gpio_dt_spec *pin_spec)
{
    int value = adc_read(pin_spec);

    if (value < 0)
        return -1.0;

    return (3.3 * value) / (4095.0);
}
