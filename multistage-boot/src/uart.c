#include "system.h"
#include "gpios.h"
#include "uart.h"

/******Static. Don't modify********/
// struct uart_pins
// {
//     gpio_port_t port;
//     uint8_t tx;
//     uint8_t rx;
//     uint8_t rts;
//     uint8_t cts;
// };

/****************************************/
int uart_configure(uart_serial_ch_t serial_ch, uart_config_t *uart_config)
{
}

unsigned char uart_read(uart_serial_ch_t serial_ch)
{
}

uint8_t uart_data_available(uart_serial_ch_t serial_ch)
{
}

int uart_read_into(uart_serial_ch_t serial_ch, unsigned char *buffer, const int size)
{
}
