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
#define CR1_PS (1 << 9)       // Parity Selection: Odd Parity
#define CR1_PCE (1 << 10)     // Parity control enable
#define CR1_M_9bits (1 << 12) // Word length:  1 Start bit, 9 Data bits, n Stop bit
#define CR1_UE (1 << 13)      // USART Enable
#define CR3_DMAR (1 << 6)     // DMA enable receiver
#define CR3_DMAT (1 << 7)     // DMA enable transmitter
#define CR3_RTSE (1 << 8)     // RTS enable
#define CR3_CTSE (1 << 9)     // CTS enabled

/*******UART REG VALUES**********/
#define CR2_STOP_1bit (0)
#define CR2_STOP_0_5bit (1)
#define CR2_STOP_2bit (2)
#define CR2_STOP_1_5bit (3)
/****************************************/

/** @brief Container for the uart pins*/
struct __uart_pins
{
    uint8_t tx;
    uint8_t rx;
    uint8_t rts;
    uint8_t cts;
    gpio_port_t port_tx;
    gpio_port_t port_rx;
    gpio_port_t port_rts;
    gpio_port_t port_cts;
};

#define _UART_INT_STRUCT(port_tx_, tx_, port_rx_, rx_, port_rts_, rts_, port_cts_, cts_) \
    ((struct __uart_pins){                                                               \
        .tx = (tx_),                                                                     \
        .rx = (rx_),                                                                     \
        .rts = (rts_),                                                                   \
        .cts = (cts_),                                                                   \
        .port_tx = (port_tx_),                                                           \
        .port_rx = (port_rx_),                                                           \
        .port_rts = (port_rts_),                                                         \
        .port_cts = (port_cts_),                                                         \
    })

#define _UART_SERIAL_SELECT(uart_serial)                                                         \
    ((uart_serial == UART_1)   ? _UART_INT_STRUCT(PORT_B, 6, PORT_A, 10, PORT_A, 12, PORT_A, 11) \
     : (uart_serial == UART_2) ? _UART_INT_STRUCT(PORT_D, 5, PORT_D, 6, PORT_D, 4, PORT_D, 3)    \
     : (uart_serial == UART_3) ? _UART_INT_STRUCT(PORT_D, 8, PORT_D, 9, PORT_D, 12, PORT_D, 11)  \
                               : _UART_INT_STRUCT(PORT_COUNT, 0, PORT_COUNT, 0, PORT_COUNT, 0, PORT_COUNT, 0))

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

static void set_stop_bit(volatile uint32_t *reg_addr, uart_config_stop_bits_t *stop_bits)
{
    // get value currently in register
    uint32_t reg_value = *reg_addr;
    // Clear bit 12&13 reg
    reg_value &= ~(0x03 << 12);

    switch (*stop_bits)
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
    *reg_addr = reg_value;
}

static void set_baud_rate(volatile uint32_t *reg_addr, uart_serial_ch_t *serial_ch, int *baudrate)
{
    uint8_t over8 = 0; // TODO: Check Over8 properly. 0: Disabled (Sampled by 16), 1: Enabled (8)
    uint32_t f_ck;

    // Move Baudrates to reliable regions and set clock speed
    switch (*serial_ch)
    {
    case UART_1:
        f_ck = APB2_FREQ;
        if (*baudrate <= 2400)
            *baudrate = 9600;
        break;
    case UART_2:
    case UART_3:
        f_ck = APB1_FREQ;
        if (*baudrate <= 1200)
            *baudrate = 2400;
        break;
    default:
        break;
    }

    uint32_t usartdiv, div_mantissa, div_fraction;

    // USARTDIV = fCK / (8 * (2 - OVER8) * baud), assuming over8 = 0
    // Multiply by 100 bring the demical point into the integer
    if (!over8) // Disabled
    {
        usartdiv = f_ck * 25 / (4 * (*baudrate));
    }
    else // (Sampling by 8)
    {
        usartdiv = f_ck * 25 / (2 * (*baudrate));
    }

    // Mantisa
    div_mantissa = usartdiv / 100;

    // Fraction
    div_fraction = usartdiv % 100;
    div_fraction *= over8 ? 8 : 16;
    div_fraction += 50;  // Round up e.g 864 -> 914
    div_fraction /= 100; // e.g 9

    // Check for overflow
    while (div_fraction >= (over8 ? 8 : 16))
    {
        div_mantissa += 1;
        div_fraction -= (over8 ? 8 : 16);
    }

    div_mantissa = (div_mantissa << 4) + div_fraction;
    div_mantissa &= ~(0xFFFF0000);

    // Set register
    *reg_addr = div_mantissa;
}

/****************************************/

int uart_configure(uart_serial_ch_t serial_ch, uart_config_t *uart_config)
{
    struct __uart_pins pins = _UART_SERIAL_SELECT(serial_ch);
    if (pins.port_tx == PORT_COUNT)
        return -1;

    // TX gpio pin setup
    gpio_dt_spec pin_specs = {
        .mode = GPIO_ALT_FUNC,
        .port = pins.port_tx,
        .pin_number = pins.tx};
    gpio_init(&pin_specs);
    gpio_set_altfunc(&pin_specs, AF7);

    // RX gpio pin setup
    pin_specs.pin_number = pins.rx;
    pin_specs.port = pins.port_rx;
    gpio_init(&pin_specs);
    gpio_set_altfunc(&pin_specs, AF7);

    // Flow control gpio pin setup
    if (uart_config->flow_ctrl == UART_CFG_FLOW_CTRL_RTS_CTS)
    {
        // rts
        pin_specs.pin_number = pins.rts;
        pin_specs.port = pins.port_rts;
        gpio_init(&pin_specs);
        gpio_set_altfunc(&pin_specs, AF7);

        // cts
        pin_specs.pin_number = pins.cts;
        pin_specs.port = pins.port_cts;
        gpio_init(&pin_specs);
        gpio_set_altfunc(&pin_specs, AF7);
    }

    // Clock Enable on bus
    uart_clock_en(&serial_ch);

    // Get the base reg
    volatile uint32_t serial_base_reg = SELECT_SERIAL_BASE(serial_ch);

    // Enable UART
    *(volatile uint32_t *)(serial_base_reg + USART_CR1) |= CR1_UE;

    // Select Word Length
    if (uart_config->data_bits == UART_CFG_DATA_BITS_8)
    {
        *(volatile uint32_t *)(serial_base_reg + USART_CR1) &= ~(CR1_M_9bits);
    }
    else if (uart_config->data_bits == UART_CFG_DATA_BITS_9)
    {
        *(volatile uint32_t *)(serial_base_reg + USART_CR1) |= CR1_M_9bits;
    }

    // Select Stop Bits
    set_stop_bit((volatile uint32_t *)(serial_base_reg + USART_CR2), &(uart_config->stop_bits));

    // Select Parity
    *(volatile uint32_t *)(serial_base_reg + USART_CR1) &= ~(CR1_PCE | CR1_PS); // Clears out and disable parity
    if (uart_config->parity == UART_CFG_PARITY_ODD)
    {
        *(volatile uint32_t *)(serial_base_reg + USART_CR1) |= (CR1_PCE | CR1_PS);
    }
    else if (uart_config->parity == UART_CFG_PARITY_EVEN)
    {
        *(volatile uint32_t *)(serial_base_reg + USART_CR1) |= CR1_PCE;
    }

    // Set Baudrate
    set_baud_rate((volatile uint32_t *)(serial_base_reg + USART_BRR), &serial_ch, &(uart_config->baudrate));

    // Select flowcontrol
    // TODO: Enable flowcontrol

    // Enable TX and RX
    //*(volatile uint32_t *)(serial_base_reg + USART_CR1) |= (CR1_RE | CR1_TE);
    *(volatile uint32_t *)(serial_base_reg + USART_CR1) |= CR1_TE;

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

int uart_write(uart_serial_ch_t serial_ch, const char *buffer)
{
    volatile uint32_t serial_base_reg = SELECT_SERIAL_BASE(serial_ch);
    if (!serial_base_reg)
        return -1;

    // Get SR & DR
    volatile uint32_t *status_reg = (volatile uint32_t *)(serial_base_reg + USART_SR);
    volatile uint32_t *data_reg = (volatile uint32_t *)(serial_base_reg + USART_DR);
    int count = 0;
    const char *ch = buffer;

    while (*ch)
    {
        // Check to see if TXE has been set by HW indicating TDR is empty
        while (!(*status_reg & SR_TXE))
        {
            continue;
        }

        *data_reg = *ch;
        count++;
        ch++;
    }

    return count;
}
