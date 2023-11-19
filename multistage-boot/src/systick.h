#ifndef SYSTICK_H_
#define SYSTICK_H_

void systick_enable();
void systick_disable();
void systick_isr(void);
unsigned int millis();

#endif
