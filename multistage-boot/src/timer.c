#include "system.h"
#include "timer.h"

/***Advanced Timer Base***/
#define TIM1_BASE (0x40010000)
#define TIM8_BASE (0x40010400)

/***Generic Timer Base***/
#define TIM2_BASE (0x40000000)
#define TIM3_BASE (0x40000400)
#define TIM4_BASE (0x40000800)
#define TIM5_BASE (0x40000C00)
#define TIM9_BASE (0x40014000)
#define TIM10_BASE (0x40014400)
#define TIM11_BASE (0x40014800)
#define TIM12_BASE (0x40001800)
#define TIM13_BASE (0x40001C00)
#define TIM14_BASE (0x40002000)

/***Basic Timer Base***/
#define TIM6_BASE (0x40001000)
#define TIM7_BASE (0x40001400)

#define SELECT_TIMER(timer_no)                                             \
    ((timer_no == TIMER_2) ? TIM2_BASE : (timer_no == TIMER_3) ? TIM3_BASE \
                                     : (timer_no == TIMER_4)   ? TIM4_BASE \
                                     : (timer_no == TIMER_5)   ? TIM5_BASE \
                                                               : 0)

/********Offsets**********/
#define TIMx_CR1 (0x00)
#define TIMx_CR2 (0x04)
#define TIMx_SMCR (0x08)
#define TIMx_DIER (0x0C)
#define TIMx_SR (0x10)
#define TIMx_EGR (0x14)
#define TIMx_CCMR1 (0x18)
#define TIMx_CCMR2 (0x1C)
#define TIMx_CCER (0x20)
#define TIMx_CNT (0x24)
#define TIMx_PSC (0x28)
#define TIMx_ARR (0x2C)
#define TIMx_CCR1 (0x34)
#define TIMx_CCR2 (0x38)
#define TIMx_CCR3 (0x3C)
#define TIMx_CCR4 (0x40)
#define TIMx_DCR (0x48)
#define TIMx_DMAR (0x4C)
#define TIM2_OR (0x50)
#define TIM5_OR (0x50)

/*******RCC*******/
#define RCC_APB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x40))
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x44))

/*************CLOCK*************/
#define APB1_FREQ (42000000)
#define APB2_FREQ (84000000)

/**************RCC EN-FLAGS*******************/
// RCC_APB1ENR ==> 42MHz
#define TIM2EN (1 << 0)
#define TIM3EN (1 << 1)
#define TIM4EN (1 << 2)
#define TIM5EN (1 << 3)
#define TIM6EN (1 << 4)
#define TIM7EN (1 << 5)
#define TIM12EN (1 << 6)
#define TIM13EN (1 << 7)
#define TIM14EN (1 << 8)
#define SELECT_TIMER_EN1(timer_no)                                   \
    ((timer_no == TIMER_2) ? TIM2EN : (timer_no == TIMER_3) ? TIM3EN \
                                  : (timer_no == TIMER_4)   ? TIM4EN \
                                  : (timer_no == TIMER_5)   ? TIM5EN \
                                                            : 0)

// RCC_APB2ENR  ==> 84MHz
#define TIM1EN (1 << 0)
#define TIM8EN (1 << 1)
#define TIM9EN (1 << 16)
#define TIM10EN (1 << 17)
#define TIM11EN (1 << 18)

/*************IRQ Number*************/
#define TIM2_IRQN (28)
#define TIM3_IRQN (29)
#define TIM4_IRQN (30)
#define TIM5_IRQN (50)
#define SELECT_TIMER_IRQN(timer_no)                                        \
    ((timer_no == TIMER_2) ? TIM2_IRQN : (timer_no == TIMER_3) ? TIM3_IRQN \
                                     : (timer_no == TIMER_4)   ? TIM4_IRQN \
                                     : (timer_no == TIMER_5)   ? TIM5_IRQN \
                                                               : -1)

/***********Flags***********/
#define TIMx_CR1_CEN (1 << 0)
#define TIMx_CR1_OPM (1 << 3)
#define TIMx_CR1_ARPE (1 << 7)
#define TIMx_DIER_UIE (1 << 0)
#define TIMx_DIER_TIE (1 << 6)
#define TIMx_DIER_UDE (1 << 8)
#define TIMx_SR_UIF (1 << 0)

/******Static. Don't modify********/
static void empty_callback(void) {} // Space holder

static void (*volatile callback_array[4])(void) = {
    empty_callback, // TIM2 callback
    empty_callback, // TIM3 callback
    empty_callback, // TIM4 callback
    empty_callback, // TIM5 callback
};

/*******GLOBALS*******/
int timer_init(timer_dt_spec *timer_spec, timer_callback_t callback)
{
    if (timer_spec->interval_ms == 0)
        return -1;

    if ((timer_spec->timer == TIMER_3 || timer_spec->timer == TIMER_4) && (timer_spec->interval_ms > 60000))
        return -1;

    // Select a prescaler that allow Reload value tick per ms
    uint32_t prescaler = (APB1_FREQ / 1000) - 1;

    // Enable RCC CLOCK
    RCC_APB1ENR |= SELECT_TIMER_EN1(timer_spec->timer);

    // Enable the IRQN
    int16_t irqn = SELECT_TIMER_IRQN(timer_spec->timer);
    if (irqn < 0)
        return -1;
    nvic_enable_irq(irqn);
    timer_register_callback(timer_spec, callback);

    // Load ARR, PSC, Set the Mode, ad Enable UIE, ARPE and CEN
    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    (*(volatile uint32_t *)(timer_reg_base + TIMx_ARR)) = timer_spec->interval_ms;
    (*(volatile uint32_t *)(timer_reg_base + TIMx_PSC)) = prescaler;
    (*(volatile uint32_t *)(timer_reg_base + TIMx_DIER)) |= TIMx_DIER_UIE;
    (*(volatile uint32_t *)(timer_reg_base + TIMx_CR1)) |= TIMx_CR1_ARPE;

    if (timer_spec->mode == ONESHOT)
    {
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CR1)) |= TIMx_CR1_OPM;
    }

    (*(volatile uint32_t *)(timer_reg_base + TIMx_CR1)) |= TIMx_CR1_CEN;

    return 0;
}

void timer_register_callback(timer_dt_spec *timer_spec, timer_callback_t callback)
{
    // Append the callback to array container
    callback_array[((int)timer_spec->timer) - 2] = callback;
}

void timer_unregister_callback(timer_dt_spec *timer_spec)
{
    // Append the callback to array container
    callback_array[((int)timer_spec->timer) - 2] = empty_callback;
}

int timer_duration_change(timer_dt_spec *timer_spec, uint32_t interval_ms)
{
    if (interval_ms == 0)
        return -1;

    if ((timer_spec->timer == TIMER_3 || timer_spec->timer == TIMER_4) && (interval_ms > 60000))
        return -1;

    // Update ARR reg
    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    (*(volatile uint32_t *)(timer_reg_base + TIMx_ARR)) = interval_ms;

    // Update the timer
    timer_spec->interval_ms = interval_ms;

    return 0;
}

/*Interrupt Service Routine for TIM2 */
void _isr_tim2(void)
{
    (*(volatile uint32_t *)(TIM2_BASE + TIMx_SR)) &= ~(TIMx_SR_UIF);
    callback_array[0]();
}

/*Interrupt Service Routine for TIM3 */
void _isr_tim3(void)
{
    (*(volatile uint32_t *)(TIM3_BASE + TIMx_SR)) &= ~(TIMx_SR_UIF);
    callback_array[1]();
}

/*Interrupt Service Routine for TIM4 */
void _isr_tim4(void)
{
    (*(volatile uint32_t *)(TIM4_BASE + TIMx_SR)) &= ~(TIMx_SR_UIF);
    callback_array[2]();
}

/*Interrupt Service Routine for TIM5 */
void _isr_tim5(void)
{
    (*(volatile uint32_t *)(TIM5_BASE + TIMx_SR)) &= ~(TIMx_SR_UIF);
    callback_array[3]();
}