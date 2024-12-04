#include "drv_uart.h"
#include "hal_uart.h"
#include <string.h> // Para memcpy

static drv_uart_t uartInstance; // Instancia única para UART

// Tamaño predeterminado del buffer
#define UART_TX_BUFFER_SIZE 256

// Buffer de transmisión
static uint8_t txBuffer[UART_TX_BUFFER_SIZE];

void drv_uart_irq(void) {
    // Verificar si hay datos en el buffer
    if (uartInstance.txHead == uartInstance.txTail) {
        uartInstance.isTransmitting = false; // No más datos que enviar
        return;
    }

    // Enviar el siguiente byte desde el buffer
    uint8_t nextByte = uartInstance.txBuffer[uartInstance.txTail];
    uartInstance.txTail = (uartInstance.txTail + 1) % uartInstance.txBufferSize;

    // Enviar el byte al hardware UART
    hal_uart_send(&nextByte, 1);

    // La transmisión de este byte continuará en hardware y/o otra interrupción
}




void iniciar_UART(uint32_t baudRate) {
    // Configuración inicial del hardware UART
    // Ejemplo de configuración para un hardware genérico:
    uartInstance.txBuffer = txBuffer;
    uartInstance.txBufferSize = UART_TX_BUFFER_SIZE;
    uartInstance.txHead = 0;
    uartInstance.txTail = 0;
    uartInstance.isTransmitting = false;

    // Configurar el hardware UART (abstracto)
    hal_uart_config_t config = {
        .type = HAL_UART_NRF52,  // Tipo de hardware, modificar según necesidad
        .baudRate = baudRate
    };
    hal_uart_init(&config);

    // Habilitar interrupciones (dependiente del hardware)
}


void enviar_UART(const uint8_t *data, uint32_t length) {
    // Validar que el tamaño de datos no exceda el buffer
    if (length > uartInstance.txBufferSize) {
        return; // Error: no se puede manejar el mensaje
    }

    // Copiar datos al buffer circular
    for (uint32_t i = 0; i < length; i++) {
        uint32_t nextHead = (uartInstance.txHead + 1) % uartInstance.txBufferSize;
        if (nextHead == uartInstance.txTail) {
            // Buffer lleno, descartar o manejar error
            return;
        }
        uartInstance.txBuffer[uartInstance.txHead] = data[i];
        uartInstance.txHead = nextHead;
    }

    // Iniciar transmisión si no está en progreso
    if (!uartInstance.isTransmitting) {
        uartInstance.isTransmitting = true;
        drv_uart_irq(); // Simular interrupción inicial
    }
}



