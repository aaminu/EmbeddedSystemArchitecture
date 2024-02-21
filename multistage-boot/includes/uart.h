#ifndef UART_H_
#define UART_H_

#include "gpios.h"

/** @brief Parity modes */
typedef enum
{
    UART_CFG_PARITY_NONE, /**< No parity */
    UART_CFG_PARITY_ODD,  /**< Odd parity */
    UART_CFG_PARITY_EVEN, /**< Even parity */
} uart_config_parity_t;

/** @brief Number of stop bits. */
typedef enum
{
    UART_CFG_STOP_BITS_0_5, /**< 0.5 stop bit */
    UART_CFG_STOP_BITS_1,   /**< 1 stop bit */
    UART_CFG_STOP_BITS_1_5, /**< 1.5 stop bits */
    UART_CFG_STOP_BITS_2,   /**< 2 stop bits */
} uart_config_stop_bits_t;

/** @brief Number of data bits. */
typedef enum
{
    UART_CFG_DATA_BITS_5, /**< 5 data bits */
    UART_CFG_DATA_BITS_6, /**< 6 data bits */
    UART_CFG_DATA_BITS_7, /**< 7 data bits */
    UART_CFG_DATA_BITS_8, /**< 8 data bits */
} uart_config_data_bits_t;

/** @brief Hardware flow control options. */
typedef enum
{
    UART_CFG_FLOW_CTRL_NONE,    /**< No flow control */
    UART_CFG_FLOW_CTRL_RTS_CTS, /**< RTS/CTS flow control */
} uart_config_flow_control_t;

/** @brief Configuration Container for UART. */
typedef struct
{
    int baudrate;
    uart_config_data_bits_t data_bits;
    uart_config_parity_t parity;
    uart_config_stop_bits_t stop_bits;
    uart_config_flow_control_t flow_ctrl;

} uart_config_t;

/**
 * @brief Configure Uart
 *
 * @param tx TX Pin Spec
 * @param rx RX Pin Spec
 * @param uart_config Configuration Container
 *
 * @return <0 if error else 0
 */
int uart_configure(gpio_dt_spec *tx, gpio_dt_spec *rx, uart_config_t *uart_config);

#endif
