#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "systick.h"
#include "uart.h"

void sys_init(void);

void main(void)
{
    // Initialize System
    sys_init();

    // Setup UART
    uart_config_t uart_config = {
        .baudrate = 9600,
        .data_bits = UART_CFG_DATA_BITS_8,
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
        .parity = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
    };
    uart_configure(UART_1, &uart_config);

    while (1)
    {
        uart_write(UART_1, "Hello STM32!!\r\n");
        delay(2000);
    }
}

void sys_init(void)
{
    sys_clock_config();
    systick_enable();

#ifdef FPU_USE
    enable_fpu();
#endif

#ifdef MPU_USE
    mpu_enable();
#endif
}
