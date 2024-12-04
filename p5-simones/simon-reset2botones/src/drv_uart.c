#include "drv_uart.h"
#include "hal_uart.h"
#include <string.h> // Para memcpy

static drv_uart_t uartInstance; // Instancia �nica para UART

// Tama�o predeterminado del buffer
#define UART_TX_BUFFER_SIZE 256

// Buffer de transmisi�n
static uint8_t txBuffer[UART_TX_BUFFER_SIZE];

void drv_uart_irq(void) {
    // Verificar si hay datos en el buffer
    if (uartInstance.txHead == uartInstance.txTail) {
        uartInstance.isTransmitting = false; // No m�s datos que enviar
        return;
    }

    // Enviar el siguiente byte desde el buffer
    uint8_t nextByte = uartInstance.txBuffer[uartInstance.txTail];
    uartInstance.txTail = (uartInstance.txTail + 1) % uartInstance.txBufferSize;

    // Enviar el byte al hardware UART
    hal_uart_send(&nextByte, 1);

    // La transmisi�n de este byte continuar� en hardware y/o otra interrupci�n
}




void iniciar_UART(uint32_t baudRate) {
    // Configuraci�n inicial del hardware UART
    // Ejemplo de configuraci�n para un hardware gen�rico:
    uartInstance.txBuffer = txBuffer;
    uartInstance.txBufferSize = UART_TX_BUFFER_SIZE;
    uartInstance.txHead = 0;
    uartInstance.txTail = 0;
    uartInstance.isTransmitting = false;

    // Configurar el hardware UART (abstracto)
    hal_uart_config_t config = {
        .type = HAL_UART_NRF52,  // Tipo de hardware, modificar seg�n necesidad
        .baudRate = baudRate
    };
    hal_uart_init(&config);

    // Habilitar interrupciones (dependiente del hardware)
}


void enviar_UART(const uint8_t *data, uint32_t length) {
    // Validar que el tama�o de datos no exceda el buffer
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

    // Iniciar transmisi�n si no est� en progreso
    if (!uartInstance.isTransmitting) {
        uartInstance.isTransmitting = true;
        drv_uart_irq(); // Simular interrupci�n inicial
    }
}



