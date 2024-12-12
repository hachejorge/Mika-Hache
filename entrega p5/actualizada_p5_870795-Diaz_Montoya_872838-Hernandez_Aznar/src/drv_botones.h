/* *****************************************************************************
 * P.H.2024: drv_botones.H
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 
#ifndef DRV_BUTTONS_INI
#define DRV_BUTTONS_INI
#include <stdint.h>
#include "rt_evento_t.h"

// Inicializamos los botones y nos suscribimos a ev_PULSAR_BOTON y ev_BOTON_RETARDO con tratar_botones
void drv_botones_iniciar(void(*callback)(EVENTO_T id, uint32_t aux), uint32_t _ev_pulsar_boton, uint32_t _ev_boton_retardo);

#endif

