#ifndef UART_H_
#define UART_H_

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
    // UART_CFG_DATA_BITS_5, /**< 5 data bits */
    // UART_CFG_DATA_BITS_6, /**< 6 data bits */
    // UART_CFG_DATA_BITS_7, /**< 7 data bits */
    UART_CFG_DATA_BITS_8, /**< 8 data bits */
    UART_CFG_DATA_BITS_9, /**< 9 data bits */
} uart_config_data_bits_t;

/** @brief Hardware flow control options. */
typedef enum
{
    UART_CFG_FLOW_CTRL_NONE,    /**< No flow control */
    UART_CFG_FLOW_CTRL_RTS_CTS, /**< RTS/CTS flow control */
} uart_config_flow_control_t;

/** @brief Serial Channel to use. */
typedef enum
{
    UART_1, /**<PORT:A, TX:9, RX:10, CTS:11, RTS:12  */
    UART_2, /**<PORT:D, TX:5, RX:6, CTS:3, RTS:4  */
    UART_3, /**<PORT:D, TX:8, RX:9, CTS:11, RTS:12  */
} uart_serial_ch_t;

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
 * @param serial_ch Serial Channel
 * @param uart_config Configuration Container
 *
 * @return <0 if error else 0
 */
int uart_configure(uart_serial_ch_t serial_ch, uart_config_t *uart_config);

/**
 * @brief Read a Single Character
 *
 * @param serial_ch Serial Channel
 * @return First item infront the RX buffer
 */
unsigned char uart_read(uart_serial_ch_t serial_ch);

/**
 * @brief Check if data is available to be read
 *
 * @param serial_ch Serial Channel
 * @return 1 if yes otherwise 0
 */
uint8_t uart_data_available(uart_serial_ch_t serial_ch);

/**
 * @brief Copy number of character specified into buffer
 *
 * @param serial_ch  Serial Channel
 * @param buffer  Recieving Buffer
 * @param size Number if characters to copy
 * @return > 0 if copied otherwise 0
 */
int uart_read_into(uart_serial_ch_t serial_ch, unsigned char *buffer, const int size);

#endif
