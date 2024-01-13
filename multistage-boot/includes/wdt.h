#ifndef WDT_H_
#define WDT_H_

#include "system.h"

/**
 * @brief Initializes the Independent watchdog timer
 * @param ms - Durtation in milliseconds.
 *
 * The current duration allowed is from 1ms up to 32secs
 *
 */
int wdt_init(uint32_t ms);

/**
 * @brief Reload
 *
 */
void wdt_feed(void);

#endif
