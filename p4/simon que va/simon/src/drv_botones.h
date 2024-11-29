
#ifndef DRV_BUTTONS_INI
#define DRV_BUTTONS_INI
#include <stdint.h>
#include "rt_evento_t.h"



/* *****************************************************************************
 * el pulsador genera un evento al cambiar de estado (toggle)
 * y genera una interrupcion de tipo: GPIOTE_IRQn
 * la RSI es GPIOTE_IRQHandler, definida en: arm_startup...
 */
void test_gpiote_button_int(void);

// Inicializamos los botones y nos suscribimos a ev_PULSAR_BOTON y ev_BOTON_RETARDO con tratar_botones
void drv_botones_iniciar(void(*callback)(EVENTO_T id, uint32_t aux), EVENTO_T ev_PULSAR_BOTON, EVENTO_T ev_BOTON_RETARDO);

// Tiene la máquina de estados donde se tratan los botones
void drv_botones_tratar(EVENTO_T evento, uint32_t auxiliar);

// Detecta si el boton está pulsado
void drv_botones_pulsado(void);

#endif

