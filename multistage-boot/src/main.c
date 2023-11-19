#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"

static int bss_variable;
static int initialized_variable = 31;

// extern int mpu_enable(void);

void main(void)
{
    // mpu_enable();
    sys_clock_config();
    systick_enable();

    while (1)
    {
        char *c = (char *)malloc(10);
        bss_variable++;
        initialized_variable++;
        unsigned int tick = millis();
        if (bss_variable % 1000 == 0)
        {
            /*Trigger a SVC*/
            asm volatile("svc 0");
        }
        for (unsigned int i; i < 150000; i++)
            ;
        free(c);
    }
}
