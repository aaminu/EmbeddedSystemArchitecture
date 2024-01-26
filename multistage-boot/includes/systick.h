#ifndef SYSTICK_H_
#define SYSTICK_H_

/**
 * @brief Enable the system ticker which keep tracks of time
 *
 */
void systick_enable(void);
/**
 * @brief Enable the system ticker which keep tracks of time
 *
 */
void systick_disable(void);

/**
 * @brief Return the time in millisecond since the system has been turned on.
 *
 * This timer uses 32bit unsigned integer to keep track and can rollover, so be careful with usage
 *
 */
unsigned int millis(void);

/**
 * @brief Induce a blocking delay of in milliseconds.
 *
 * @param delay_ms - Blocking delay in milliseconds
 *
 */
void delay(uint32_t delay_ms);

#endif
