#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *txBuffer; // Buffer de transmisión
    uint32_t txBufferSize;
    volatile uint32_t txHead;
    volatile uint32_t txTail;
    bool isTransmitting; // Indica si hay transmisión en progreso
} drv_uart_t;

// Inicialización de UART
void iniciar_UART(uint32_t baudRate);

// Envío de datos
void enviar_UART(const uint8_t *data, uint32_t length);

// Callback de interrupción de TX (para transmisión no bloqueante)
void drv_uart_irq(void);

#endif // DRV_UART_H
