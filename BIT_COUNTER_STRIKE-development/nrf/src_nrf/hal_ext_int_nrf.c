#include "hal_ext_int.h"
#include <nrf.h>
#include "rt_fifo.h"


static void (*callback_func)(uint32_t pin) = NULL; // Función de callback almacenada

/**
 * Inicializa el módulo y almacena la función de callback para interrupciones.
 * funcion_IRQ Puntero a la función que se ejecutará en la interrupción.
 */
void hal_ext_int_iniciar(void (*funcion_IRQ)(uint32_t pin)) {
    callback_func = funcion_IRQ;
}

/**
 * Habilita las interrupciones en el pin especificado.
 * pin Pin en el que se habilitarán las interrupciones.
 */
void hal_habilitar_int(uint32_t pin) {
    // Configurar GPIOTE para generar eventos en el pin
    uint32_t channel = pin % 8; // Seleccionar canal válido (0-7) basado en el pin
    NRF_GPIOTE->CONFIG[channel] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
                                  (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) |
                                  (pin << GPIOTE_CONFIG_PSEL_Pos);

    // Habilitar interrupciones en el canal
    NRF_GPIOTE->INTENSET = (1 << channel);

    // Habilitar la IRQ en el NVIC
    NVIC_EnableIRQ(GPIOTE_IRQn);
}

/**
 * Deshabilita las interrupciones en el pin especificado.
 * pin Pin en el que se deshabilitarán las interrupciones.
 */
void hal_deshabilitar_int(uint32_t pin) {
    uint32_t channel = pin % 8; // Seleccionar canal válido (0-7) basado en el pin

    // Deshabilitar interrupciones en el canal
    NRF_GPIOTE->INTENCLR = (1 << channel);

    // Limpiar configuración del canal
    NRF_GPIOTE->CONFIG[channel] = 0;

    // Deshabilitar la IRQ en el NVIC si no quedan canales habilitados
    if (NRF_GPIOTE->INTENSET == 0) {
        NVIC_DisableIRQ(GPIOTE_IRQn);
    }
}

/**
 * Habilita la funcionalidad de despertar del sistema.
 */
void hal_habilitar_despertar(void) {
    // Habilitar detección de eventos de interrupción en modo bajo consumo
    NRF_POWER->SYSTEMOFF = 0; // Ejemplo: evitar modo apagado total.
}

/**
 * Deshabilita la funcionalidad de despertar del sistema.
 */
void hal_deshabilitar_despertar(void) {
    // Opcional: No se requiere normalmente configuración explícita para el despertar
}

/**
 * Manejador de interrupciones GPIOTE.
 */
void GPIOTE_IRQHandler(void) {
    for (uint32_t channel = 0; channel < 8; channel++) {
        if (NRF_GPIOTE->EVENTS_IN[channel]) {
            NRF_GPIOTE->EVENTS_IN[channel] = 0; // Limpiar evento

			//dummy = NRF_GPIOTE->EVENTS_IN[channel];
  			//dummy; 

            // Ejecutar la función de callback con el pin asociado al canal
            if (callback_func) {
                callback_func(NRF_GPIOTE->CONFIG[channel] & GPIOTE_CONFIG_PSEL_Msk);
            }
        }
    }
}

/*
void hal_habilitar_despertar(){
	NRF_POWER->TASKS_LOWPWR = 1;
}

void hal_deshabilitar_despertar(){
	NRF_POWER->TASKS_LOWPWR = 0;
}
*/

