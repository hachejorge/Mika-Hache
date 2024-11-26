#ifndef HAL_EXT_INT
#define HAL_EXT_INT

#include "rt_evento_t.h"
#include <stdint.h>

/**
 * Inicializa el módulo y almacena la función de callback para interrupciones.
 * funcion_IRQ Puntero a la función que se ejecutará en la interrupción.
 */
void hal_ext_int_iniciar(void (*funcion_IRQ)(EVENTO_T evento, uint32_t pin));

/**
 * Habilita las interrupciones en el pin especificado.
 * Pin en el que se habilitarán las interrupciones.
 */
void hal_habilitar_int(uint32_t pin);

/**
 * Deshabilita las interrupciones en el pin especificado.
 * Pin en el que se deshabilitarán las interrupciones.
 */
void hal_deshabilitar_int(uint32_t pin);

/**
* Limpia todos los canales, los eventos en ellos y deshabilita las interrupciones en el canal
 */
void hal_habilitar_despertar(uint32_t pin);

/*
* Configura el pin para que despierte el microcontrolador cuando el nivel sea bajo (0 lógico).
*/
void hal_deshabilitar_despertar(uint32_t pin);

/**
* Limpia todos los canales, los eventos en ellos y deshabilita las interrupciones en el canal
 */
void hal_ext_int_limpiar(void); 

#endif
