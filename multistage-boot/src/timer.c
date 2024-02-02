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
#define APB1_TIMER_FREQ (APB1_FREQ * 2)
#define APB2_TIMER_FREQ (APB2_FREQ * 2)

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
#define TIMx_CR1_UDIS (1 << 1)
#define TIMx_CR1_URS (1 << 1)
#define TIMx_CR1_OPM (1 << 3)
#define TIMx_CR1_DIR (1 << 4)
#define TIMx_CR1_ARPE (1 << 7)
#define TIMx_DIER_UIE (1 << 0)
#define TIMx_DIER_TIE (1 << 6)
#define TIMx_DIER_UDE (1 << 8)
#define TIMx_SR_UIF (1 << 0)
#define TIMx_EGR_UG (1 << 0)

#define TIMx_CCER_CC1E (1 << 0)
#define TIMx_CCER_CC2E (1 << 4)
#define TIMx_CCER_CC3E (1 << 8)
#define TIMx_CCER_CC4E (1 << 12)
#define SELECT_CCER_EN(timer_ch)                                                           \
    ((timer_ch == TIMERx_CH1) ? TIMx_CCER_CC1E : (timer_ch == TIMERx_CH2) ? TIMx_CCER_CC2E \
                                             : (timer_ch == TIMERx_CH3)   ? TIMx_CCER_CC3E \
                                             : (timer_ch == TIMERx_CH4)   ? TIMx_CCER_CC4E \
                                                                          : 0)
#define TIMx_CCER_CC1P (1 << 1)
#define TIMx_CCER_CC2P (1 << 5)
#define TIMx_CCER_CC3P (1 << 9)
#define TIMx_CCER_CC4P (1 << 13)
#define SELECT_CCER_P(timer_ch)                                                            \
    ((timer_ch == TIMERx_CH1) ? TIMx_CCER_CC1P : (timer_ch == TIMERx_CH2) ? TIMx_CCER_CC2P \
                                             : (timer_ch == TIMERx_CH3)   ? TIMx_CCER_CC3P \
                                             : (timer_ch == TIMERx_CH4)   ? TIMx_CCER_CC4P \
                                                                          : 0)
#define TIMx_CCER_CC1NP (1 << 3)
#define TIMx_CCER_CC2NP (1 << 7)
#define TIMx_CCER_CC3NP (1 << 11)
#define TIMx_CCER_CC4NP (1 << 15)
#define SELECT_CCER_NP(timer_ch)                                                             \
    ((timer_ch == TIMERx_CH1) ? TIMx_CCER_CC1NP : (timer_ch == TIMERx_CH2) ? TIMx_CCER_CC2NP \
                                              : (timer_ch == TIMERx_CH3)   ? TIMx_CCER_CC3NP \
                                              : (timer_ch == TIMERx_CH4)   ? TIMx_CCER_CC4NP \
                                                                           : 0)

/******Static. Don't modify********/
static void empty_callback(void) {} // Space holder

static void (*volatile callback_array[4])(void) = {
    empty_callback, // TIM2 callback
    empty_callback, // TIM3 callback
    empty_callback, // TIM4 callback
    empty_callback, // TIM5 callback
};

/*******GLOBALS*******/
int timer_init(const timer_dt_spec *timer_spec, uint32_t interval_ms)
{
    if (interval_ms < 0)
        return -1;

    if ((timer_spec->timer == TIMER_3 || timer_spec->timer == TIMER_4) && (interval_ms > T_SEC(30)))
        return -1;

    if ((timer_spec->timer == TIMER_2 || timer_spec->timer == TIMER_5) && (interval_ms > T_HRS(24)))
        return -1;

    // Enable RCC CLOCK
    RCC_APB1ENR |= SELECT_TIMER_EN1(timer_spec->timer);

    // Set Timer CR1 to reset state
    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    (*(volatile uint32_t *)(timer_reg_base + TIMx_CR1)) = 0;

    if (interval_ms > 0)
    {
        // Select a prescaler that allow Reload value tick per ms
        uint32_t prescaler = (APB1_FREQ / 1000) - 1;

        // Load ARR, PSC, Set the Mode
        (*(volatile uint32_t *)(timer_reg_base + TIMx_ARR)) = (interval_ms * 2) - 1;
        (*(volatile uint32_t *)(timer_reg_base + TIMx_PSC)) = prescaler;

        if (timer_spec->mode == ONESHOT)
        {
            (*(volatile uint32_t *)(timer_reg_base + TIMx_CR1)) |= TIMx_CR1_OPM;
        }

        // Generate Update Event
        timer_generate_uev(timer_spec);
        // Clear Update Flag
        timer_clear_update_flag(timer_spec);

        return 0;
    }

    return 0;
}

int timer_set_arr(const timer_dt_spec *timer_spec, uint32_t arr)
{
    if ((timer_spec->timer != TIMER_2 || timer_spec->timer != TIMER_5) && (arr > (uint16_t)-1))
        return -1;

    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    (*(volatile uint32_t *)(timer_reg_base + TIMx_ARR)) = arr - 1;

    // Generate Update Event
    timer_generate_uev(timer_spec);
    // Clear Update Flag
    timer_clear_update_flag(timer_spec);

    return 0;
}

void timer_set_prescaler(const timer_dt_spec *timer_spec, uint16_t prescaler)
{
    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    (*(volatile uint32_t *)(timer_reg_base + TIMx_PSC)) = prescaler - 1;

    // Generate Update Event
    timer_generate_uev(timer_spec);
    // Clear Update Flag
    timer_clear_update_flag(timer_spec);
}

void timer_generate_uev(const timer_dt_spec *timer_spec)
{
    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    // Generate Update Event
    (*(volatile uint32_t *)(timer_reg_base + TIMx_EGR)) |= TIMx_EGR_UG;
}

void timer_clear_update_flag(const timer_dt_spec *timer_spec)
{
    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    // Clear Update Flag
    (*(volatile uint32_t *)(timer_reg_base + TIMx_SR)) &= ~(TIMx_SR_UIF);
}

void timer_start(const timer_dt_spec *timer_spec)
{

    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);

    // Enable CEN
    (*(volatile uint32_t *)(timer_reg_base + TIMx_CR1)) |= TIMx_CR1_CEN;
}

void timer_register_callback(const timer_dt_spec *timer_spec, timer_callback_t callback)
{
    // Append the callback to array container
    callback_array[((int)timer_spec->timer) - 2] = callback;

    // Get IRQN
    int16_t irqn = SELECT_TIMER_IRQN(timer_spec->timer);
    if (irqn < 0)
        return;
    nvic_enable_irq(irqn);

    // Set UIE & Enable Interrupt
    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    (*(volatile uint32_t *)(timer_reg_base + TIMx_DIER)) |= TIMx_DIER_UIE;
}

void timer_unregister_callback(const timer_dt_spec *timer_spec)
{
    // Append the callback to array container
    callback_array[((int)timer_spec->timer) - 2] = empty_callback;
}

int timer_duration_change(const timer_dt_spec *timer_spec, uint32_t interval_ms)
{
    if (interval_ms == 0)
        return -1;

    if ((timer_spec->timer == TIMER_3 || timer_spec->timer == TIMER_4) && (interval_ms > 60000))
        return -1;

    // Update ARR reg
    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    (*(volatile uint32_t *)(timer_reg_base + TIMx_ARR)) = interval_ms;

    return 0;
}

unsigned int timer_get_counter(const timer_dt_spec *timer_spec)
{

    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);

    return ((*(volatile uint32_t *)(timer_reg_base + TIMx_CNT)) + 1) / 2;
}

void timer_ccer_enable(const timer_dt_spec *timer_spec, timer_ch_t timerx_ch, timer_ccer_t ccer_type)
{
    // Clear the require reg first
    timer_ccer_disable(timer_spec, timerx_ch, ccer_type);

    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    switch (ccer_type)
    {
    case TIMx_CCER_EN:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCER)) |= SELECT_CCER_EN(timerx_ch);
        break;
    case TIMx_CCER_P:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCER)) |= SELECT_CCER_P(timerx_ch);
        break;
    case TIMx_CCER_NP:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCER)) |= SELECT_CCER_NP(timerx_ch);
        break;
    default:
        break;
    }
}

void timer_ccer_disable(const timer_dt_spec *timer_spec, timer_ch_t timerx_ch, timer_ccer_t ccer_type)
{
    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    switch (ccer_type)
    {
    case TIMx_CCER_EN:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCER)) &= ~SELECT_CCER_EN(timerx_ch);
        break;
    case TIMx_CCER_P:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCER)) &= ~SELECT_CCER_P(timerx_ch);
        break;
    case TIMx_CCER_NP:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCER)) &= ~SELECT_CCER_NP(timerx_ch);
        break;
    default:
        break;
    }
}

void timer_ccrx_set(const timer_dt_spec *timer_spec, timer_ch_t timerx_ch, uint32_t value)
{

    uint32_t timer_reg_base = (uint32_t)SELECT_TIMER(timer_spec->timer);
    switch (timerx_ch)
    {
    case TIMERx_CH1:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCR1)) = value;
        break;
    case TIMERx_CH2:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCR2)) = value;
        break;
    case TIMERx_CH3:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCR3)) = value;
        break;
    case TIMERx_CH4:
        (*(volatile uint32_t *)(timer_reg_base + TIMx_CCR4)) = value;
        break;
    default:
        break;
    }
}

/*********************************************EXTERNAL ISRs**************************************************************/
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
