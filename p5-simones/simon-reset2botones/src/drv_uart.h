#ifndef DRV_UART_H
#define DRV_UART_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *txBuffer; // Buffer de transmisi�n
    uint32_t txBufferSize;
    volatile uint32_t txHead;
    volatile uint32_t txTail;
    bool isTransmitting; // Indica si hay transmisi�n en progreso
} drv_uart_t;

// Inicializaci�n de UART
void iniciar_UART(uint32_t baudRate);

// Env�o de datos
void enviar_UART(const uint8_t *data, uint32_t length);

// Callback de interrupci�n de TX (para transmisi�n no bloqueante)
void drv_uart_irq(void);

#endif // DRV_UART_H
