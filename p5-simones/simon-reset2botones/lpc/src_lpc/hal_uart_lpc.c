#include "hal_uart.h"
#include "LPC210x.h"

void hal_uart_init(const hal_uart_config_t *config) {
    if (config->type == HAL_UART_LPC2105) {
        U1LCR = 0x83; // Habilitar acceso a DLL y DLM
        U1DLL = config->baudRate & 0xFF;
        U1DLM = (config->baudRate >> 8) & 0xFF;
        U1LCR = 0x03; // Configuración 8N1
        U1FCR = 0x01; // Habilitar FIFO
    }
}

void hal_uart_send(const uint8_t *data, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        while (!(U1LSR & (1 << 5))); // Esperar hasta THR vacío
        U1THR = data[i];
    }
}
