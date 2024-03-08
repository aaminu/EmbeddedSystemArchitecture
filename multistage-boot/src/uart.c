#include "system.h"
#include "gpios.h"
#include "uart.h"

/*******RCC*******/
#define RCC_APB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x40))
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x44))

/******PORTS_ENABLE*******/
#define USART1EN (1 << 4)
#define USART2EN (1 << 17)
#define USART3EN (1 << 18)

#define ENABLE_SERIAL(serial_ch)                                         \
    ((serial_ch == UART_1) ? USART1EN : (serial_ch == UART_2) ? USART2EN \
                                    : (serial_ch == UART_3)   ? USART3EN \
                                                              : 0)

/*******UART BASE**********/
#define USART1_BASE (0x40011000)
#define USART2_BASE (0x40004400)
#define USART3_BASE (0x40004800)

#define SELECT_SERIAL_BASE(serial_ch)                                          \
    ((serial_ch == UART_1) ? USART1_BASE : (serial_ch == UART_2) ? USART2_BASE \
                                       : (serial_ch == UART_3)   ? USART3_BASE \
                                                                 : 0)

/*******UART REG**********/
#define USART_SR (0x00) // Status Registers
#define USART_DR (0x04)
#define USART_BRR (0x08)
#define USART_CR1 (0x0C)
#define USART_CR2 (0x10)
#define USART_CR3 (0x14)
#define USART_GTPR (0x18)

/*******UART REG FLAGS**********/
#define SR_PE (1 << 0)        // Parity Error
#define SR_FE (1 << 1)        // Framing Error
#define SR_ORE (1 << 3)       // Overrun Error
#define SR_RXNE (1 << 5)      // Read Data Register not Empty
#define SR_TC (1 << 6)        // Transmission Complete
#define SR_TXE (1 << 7)       // Transmission Data register empty
#define SR_CTS (1 << 9)       // CTS Flags
#define CR1_SBK (1 << 0)      // Send Break
#define CR1_RE (1 << 2)       // Receiver Enable
#define CR1_TE (1 << 3)       // Transmitter Enable
#define CR1_PS (1 << 9)       // Parity Selection
#define CR1_PCE (1 << 10)     // Parity control enable
#define CR1_M_8bits (0 << 12) // Word length:  1 Start bit, 8 Data bits, n Stop bit
#define CR1_M_9bits (1 << 12) // Word length:  1 Start bit, 9 Data bits, n Stop bit
#define CR1_UE (1 << 14)      // USART Enable

/*******UART REG VALUES**********/
#define CR2_STOP_1bit (0)
#define CR2_STOP_0_5bit (1)
#define CR2_STOP_2bit (2)
#define CR2_STOP_1_5bit (3)
/****************************************/

/** @brief Container for the uart pins*/
struct __uart_pins
{
    gpio_port_t port;
    uint8_t tx;
    uint8_t rx;
    uint8_t rts;
    uint8_t cts;
};

#define _UART_INT_STRUCT(port_, tx_, rx_, rts_, cts_) \
    ((struct __uart_pins){.port = (port_),            \
                          .tx = (tx_),                \
                          .rx = (rx_),                \
                          .rts = (rts_),              \
                          .cts = (cts_)})

#define _UART_SERIAL_SELECT(uart_serial)                                 \
    ((uart_serial == UART_1)   ? _UART_INT_STRUCT(PORT_A, 9, 10, 12, 11) \
     : (uart_serial == UART_2) ? _UART_INT_STRUCT(PORT_D, 5, 6, 4, 3)    \
     : (uart_serial == UART_3) ? _UART_INT_STRUCT(PORT_D, 8, 9, 12, 11)  \
                               : _UART_INT_STRUCT(PORT_COUNT, 0, 0, 0, 0))

/******Static. Don't modify********/

static void uart_clock_en(uart_serial_ch_t *serial_ch)
{
    switch (*serial_ch)
    {
    case UART_1:
        RCC_APB2ENR |= ENABLE_SERIAL(*serial_ch);
        break;
    case UART_2:
    case UART_3:
        RCC_APB1ENR |= ENABLE_SERIAL(*serial_ch);
        break;

    default:
        break;
    }
}

static void set_stop_bit(uart_serial_ch_t *serial_ch, uart_config_t *uart_config)
{
    volatile uint32_t serial_base_reg = SELECT_SERIAL_BASE(*serial_ch);

    // If serial_channel is an error
    if (!serial_base_reg)
        return;

    serial_base_reg += USART_CR2;

    // get value currently in register
    volatile uint32_t reg_value = *(volatile uint32_t *)serial_base_reg;
    // Clear bit 12&13 reg
    reg_value &= ~(0x03 << 12);

    switch (uart_config->stop_bits)
    {
    case UART_CFG_STOP_BITS_0_5:
        reg_value |= (CR2_STOP_0_5bit << 12);
        break;
    case UART_CFG_STOP_BITS_1:
        // Already set by clearing reg_value
        break;
    case UART_CFG_STOP_BITS_1_5:
        reg_value |= (CR2_STOP_1_5bit << 12);
        break;
    case UART_CFG_STOP_BITS_2:
        reg_value |= (CR2_STOP_2bit << 12);
        break;
    default:
        break;
    }

    // Save value back to address
    *(volatile uint32_t *)serial_base_reg = reg_value;
}

/****************************************/

int uart_configure(uart_serial_ch_t serial_ch, uart_config_t *uart_config)
{
    // struct __uart_pins pins = _UART_SERIAL_SELECT(serial_ch);
    // if (pins.port == PORT_COUNT)
    //     return -1;

    // // TX gpio pin setup
    // volatile gpio_dt_spec pin_specs = {
    //     .mode = GPIO_ALT_FUNC,
    //     .port = pins.port,
    //     .pin_number = pins.tx};
    // gpio_init(&pin_specs);
    // gpio_set_altfunc(&pin_specs, AF7);

    // // RX TX gpio pin setup
    // pin_specs.pin_number = pins.rx;
    // gpio_init(&pin_specs);
    // gpio_set_altfunc(&pin_specs, AF7);

    // // Flow control gpio pin setup
    // if (uart_config->flow_ctrl == UART_CFG_FLOW_CTRL_NONE)
    // {
    //     // rts
    //     pin_specs.pin_number = pins.rts;
    //     gpio_init(&pin_specs);
    //     gpio_set_altfunc(&pin_specs, AF7);

    //     // cts
    //     pin_specs.pin_number = pins.cts;
    //     gpio_init(&pin_specs);
    //     gpio_set_altfunc(&pin_specs, AF7);
    // }

    // // Clock Enable on bus
    // uart_clock_en(&serial_ch);

    return 0;
}

unsigned char uart_read(uart_serial_ch_t serial_ch)
{

    return 'a'; // placeholder
}

uint8_t uart_data_available(uart_serial_ch_t serial_ch)
{

    return 0;
}

int uart_read_into(uart_serial_ch_t serial_ch, unsigned char *buffer, const int size)
{

    return 0;
}
