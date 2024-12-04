#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    HAL_UART_NRF52,
    HAL_UART_LPC2105
} hal_uart_type_t;

typedef struct {
    hal_uart_type_t type;
    uint32_t baudRate;
} hal_uart_config_t;

void hal_uart_init(const hal_uart_config_t *config);
void hal_uart_send(const uint8_t *data, uint32_t length);

#endif // HAL_UART_H
