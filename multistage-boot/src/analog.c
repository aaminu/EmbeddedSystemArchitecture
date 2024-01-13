#include <stdlib.h>
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
#define ADC2_BASE (ADC1_BASE + 0x100)
#define ADC3_BASE (ADC1_BASE + 0x200)
#define ADC_COMMON_REG_BASE (ADC1_BASE + 0x300)
#define SELECT_ADC_CONTR_BASE(controller)                                \
    ((controller == ADC1) ? ADC1_BASE : (controller == ADC2) ? ADC2_BASE \
                                    : (controller == ADC3)   ? ADC3_BASE \
                                                             : 0)

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

/**********ADC COMMON REG*************/
#define ADC_CSR (*(volatile uint32_t *)(ADC_COMMON_REG_BASE + 0x00))
#define ADC_CCR (*(volatile uint32_t *)(ADC_COMMON_REG_BASE + 0x04))
#define ADC_CDR (*(volatile uint32_t *)(ADC_COMMON_REG_BASE + 0x08))

/*****FLAGS*****/

#define ADC_SR_EOC (1 << 1)       // Regular channel end of conversion
#define ADC_CR1_SCAN (1 << 8)     // Scan mode enabled
#define ADC_CR1_DISCEN (1 << 11)  // Discontinuous mode on regular channels
#define ADC_CR2_ADON (1 << 0)     // ADC On
#define ADC_CR2_CONT (1 << 1)     // Continous conversion enabled
#define ADC_CR2_SWSTART (1 << 30) // Start conversion of regular channel

/***ADC Prescalers***/

#define ADC_CCR_ADCPRE_PCLK_DIV2 (0x00 << 16)
#define ADC_CCR_ADCPRE_PCLK_DIV4 (0x01 << 16)
#define ADC_CCR_ADCPRE_PCLK_DIV6 (0x02 << 16)
#define ADC_CCR_ADCPRE_PCLK_DIV8 (0x03 << 16)
#define ADC_CCR_TSVREFE (0x01 << 23)

/*****SAMPLING TIME*****/

#define ADC_SMPR_3_CYCLE (0x00)
#define ADC_SMPR_15_CYCLE (0x01)
#define ADC_SMPR_28_CYCLE (0x02)
#define ADC_SMPR_56_CYCLE (0x03)
#define ADC_SMPR_84_CYCLE (0x04)
#define ADC_SMPR_112_CYCLE (0x05)
#define ADC_SMPR_144_CYCLE (0x06)
#define ADC_SMPR_480_CYCLE (0x07)

#define SELECT_SMP(smp)                                                                                         \
    ((smp == ADC_SAMPLETIME_3CYCLES) ? ADC_SMPR_3_CYCLE : (smp == ADC_SAMPLETIME_15CYCLES) ? ADC_SMPR_15_CYCLE  \
                                                      : (smp == ADC_SAMPLETIME_28CYCLES)   ? ADC_SMPR_28_CYCLE  \
                                                      : (smp == ADC_SAMPLETIME_56CYCLES)   ? ADC_SMPR_56_CYCLE  \
                                                      : (smp == ADC_SAMPLETIME_84CYCLES)   ? ADC_SMPR_84_CYCLE  \
                                                      : (smp == ADC_SAMPLETIME_112CYCLES)  ? ADC_SMPR_112_CYCLE \
                                                      : (smp == ADC_SAMPLETIME_144CYCLES)  ? ADC_SMPR_144_CYCLE \
                                                                                           : ADC_SMPR_480_CYCLE)

/***DATA ALIGN*********/

#define ADC_CR2_ALIGNR (0 << 11)
#define ADC_CR2_ALIGNL (1 << 11)

/********OTHERS*********/
#define TEMPERATURE_CHANNEL (16)
#define CHANNEL_ERROR (100)
#define ANALOG_VAL_ERROR (1)
#define TEMP_VAL_ERROR (50.0)

#define IS_ADC1_CLK_EN (0x01)
#define IS_ADC2_CLK_EN (0x02)
#define IS_ADC3_CLK_EN (0x04)

/**************Statics**************************/

static uint8_t is_adc_controller_clk_en = 0; // Keep track of ADC controller clock enabled, 0x01(ADC1), 0x02(ADC2), 0x40(ADC3)

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

    if ((pin_spec->port == PORT_A) && (pin_spec->pin_number <= 7))
    {
        channel = pin_spec->pin_number;
    }
    else if ((pin_spec->port == PORT_B) && (pin_spec->pin_number <= 1))
    {
        channel = pin_spec->pin_number + 8;
    }
    else if ((pin_spec->port == PORT_C) && (pin_spec->pin_number <= 5))
    {
        channel = pin_spec->pin_number + 10;
    }

    return channel;
}

static void adc_controller_clk_en(adc_controller_t adc_controller)
{
    if ((adc_controller == ADC1) && ((is_adc_controller_clk_en & IS_ADC1_CLK_EN) == 0))
    {
        RCC_APB2ENR |= ADC1EN;
        is_adc_controller_clk_en |= IS_ADC1_CLK_EN;
    }
    else if ((adc_controller == ADC2) && ((is_adc_controller_clk_en & IS_ADC2_CLK_EN) == 0))
    {
        RCC_APB2ENR |= ADC2EN;
        is_adc_controller_clk_en |= IS_ADC2_CLK_EN;
    }
    else if ((adc_controller == ADC3) && ((is_adc_controller_clk_en & IS_ADC3_CLK_EN) == 0))
    {
        RCC_APB2ENR |= ADC3EN;
        is_adc_controller_clk_en |= IS_ADC3_CLK_EN;
    }
}

static void set_adc_prescaler(adc_clock_prescaler_t adc_prescaler)
{
    // Clear the reg
    ADC_CCR &= ~(0x03 << 16);

    switch (adc_prescaler)
    {
    case ADC_PCLK_DIV2:
        ADC_CCR |= ADC_CCR_ADCPRE_PCLK_DIV2;
        break;
    case ADC_PCLK_DIV4:
        ADC_CCR |= ADC_CCR_ADCPRE_PCLK_DIV4;
        break;
    case ADC_PCLK_DIV6:
        ADC_CCR |= ADC_CCR_ADCPRE_PCLK_DIV6;
        break;
    case ADC_PCLK_DIV8:
        ADC_CCR |= ADC_CCR_ADCPRE_PCLK_DIV8;
        break;
    default:
        break;
    }
}

static void adc_mode_set(adc_controller_t adc_controller, adc_mode_t mode, adc_mode_trigger_t en)
{
    uint32_t controller_base = SELECT_ADC_CONTR_BASE(adc_controller);
    if (!controller_base)
        return;

    switch (mode)
    {
    case ADC_SINGLE_MODE:
        (*(volatile uint32_t *)(controller_base + ADC_CR2)) &= ~(ADC_CR2_CONT);
        break;

    case ADC_CONTINOUS_MODE:
        if (en)
        {
            (*(volatile uint32_t *)(controller_base + ADC_CR2)) |= ADC_CR2_CONT;
            break;
        }

        (*(volatile uint32_t *)(controller_base + ADC_CR2)) &= ~(ADC_CR2_CONT);
        break;

    case ADC_SCAN_MODE:
        if (en)
        {
            (*(volatile uint32_t *)(controller_base + ADC_CR1)) |= ADC_CR1_SCAN;
            break;
        }

        (*(volatile uint32_t *)(controller_base + ADC_CR1)) &= ~(ADC_CR1_SCAN);
        break;

    case ADC_DISCONTINOUS_MODE:
        if (en)
        {
            (*(volatile uint32_t *)(controller_base + ADC_CR1)) |= ADC_CR1_DISCEN;
            break;
        }

        (*(volatile uint32_t *)(controller_base + ADC_CR1)) &= ~(ADC_CR1_DISCEN);
        break;

    default:
        break;
    }
}

static void adc_sampletime_set(adc_controller_t adc_controller, adc_sampletime_t sample_time, uint8_t channel)
{
    uint32_t controller_base = SELECT_ADC_CONTR_BASE(adc_controller);
    if (!controller_base)
        return;

    if (channel < 9)
    {
        // clear
        (*(volatile uint32_t *)(controller_base + ADC_SMPR2)) &= ~(0x07 << (channel * 3));
        (*(volatile uint32_t *)(controller_base + ADC_SMPR2)) |= (SELECT_SMP(sample_time) << (channel * 3));
    }
    else
    {
        // clear
        (*(volatile uint32_t *)(controller_base + ADC_SMPR1)) &= ~(0x07 << ((channel - 10) * 3));
        (*(volatile uint32_t *)(controller_base + ADC_SMPR1)) |= (SELECT_SMP(sample_time) << ((channel - 10) * 3));
    }
}

static void adc_reg_switch(adc_controller_t adc_controller, adc_mode_trigger_t en)
{
    uint32_t controller_base = SELECT_ADC_CONTR_BASE(adc_controller);
    if (!controller_base)
        return;

    if (en)
    {
        (*(volatile uint32_t *)(controller_base + ADC_CR2)) |= ADC_CR2_ADON;
        return;
    }

    (*(volatile uint32_t *)(controller_base + ADC_CR2)) &= ~(ADC_CR2_ADON);
}

static int adc_singleconv_read(adc_controller_t adc_controller, uint8_t channel)
{

    uint32_t controller_base = SELECT_ADC_CONTR_BASE(adc_controller);
    if (!controller_base)
        return -1;

    // Disable ADC
    adc_reg_switch(adc_controller, DISABLE);

    // Set Channel to SQR_x
    (*(volatile uint32_t *)(controller_base + ADC_SQR3)) &= ~(0x1F);
    (*(volatile uint32_t *)(controller_base + ADC_SQR3)) |= channel;

    // Enable ADC
    adc_reg_switch(adc_controller, ENABLE);

    // Start Conversion
    (*(volatile uint32_t *)(controller_base + ADC_CR2)) |= ADC_CR2_SWSTART;

    // Wait for Conversion to Start
    while ((*(volatile uint32_t *)(controller_base + ADC_CR2)) & ADC_CR2_SWSTART)
    {
        ;
    }

    // Wait for End of Conversion
    while (((*(volatile uint32_t *)(controller_base + ADC_SR)) & ADC_SR_EOC) == 0)
    {
        ;
    }

    return (int)(*(volatile uint32_t *)(controller_base + ADC_DR));
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

    // check and Enable ADC CLK if required
    adc_controller_clk_en(ADC1);

    // Initial GPIO as ADC
    gpio_init(pin_spec);

    // Disable ADC
    adc_reg_switch(ADC1, DISABLE);

    // Set the Clock Prescaler
    set_adc_prescaler(ADC_PCLK_DIV2);

    // Disable Scan Mode
    adc_mode_set(ADC1, ADC_SCAN_MODE, DISABLE);

    // Disable Continous Mode
    adc_mode_set(ADC1, ADC_SINGLE_MODE, ENABLE);

    // Sampling Frequency
    adc_sampletime_set(ADC1, ADC_SAMPLETIME_480CYCLES, channel);

    // Enable ADC
    adc_reg_switch(ADC1, ENABLE);
}

int adc_read(gpio_dt_spec *pin_spec)
{
    // Check if Pin is suitable for ADC
    int channel = adc_channel(pin_spec);
    if (channel < 0)
        return -ANALOG_VAL_ERROR;

    return adc_singleconv_read(ADC1, channel);
}

float adc_read_volt(gpio_dt_spec *pin_spec)
{
    int value = adc_read(pin_spec);

    if (value < 0)
        return -1.0;

    return (2.79f * value) / (4095.0f);
}

float adc_temperature(void)
{

    // check and Enable ADC CLK if required
    adc_controller_clk_en(ADC1);

    // Disable ADC
    adc_reg_switch(ADC1, DISABLE);

    // Set the Clock Prescaler
    set_adc_prescaler(ADC_PCLK_DIV2);

    // Disable Scan Mode
    adc_mode_set(ADC1, ADC_SCAN_MODE, DISABLE);

    // Disable Continous Mode
    adc_mode_set(ADC1, ADC_SINGLE_MODE, ENABLE);

    // Sampling Frequency, Temp Sensor is on ADC_IN16
    adc_sampletime_set(ADC1, ADC_SAMPLETIME_480CYCLES, TEMPERATURE_CHANNEL);

    // Enable ADC_CCR TSVREFE
    ADC_CCR |= ADC_CCR_TSVREFE;

    // Enable ADC
    adc_reg_switch(ADC1, ENABLE);

    int value = adc_singleconv_read(ADC1, TEMPERATURE_CHANNEL);
    if (value == -ANALOG_VAL_ERROR)
        return -TEMP_VAL_ERROR;

    float v_sense = (2.79f * value) / (4095.0f);

    return ((v_sense - 0.76f) * 100.0f) / (2.5f) + 25.0f;
}
