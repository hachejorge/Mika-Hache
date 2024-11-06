/* *****************************************************************************
 * P.H.2024: hal_gpio_nrf.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */

#include "rt_evento_t.h"

#ifndef DRV_BOTONES
#define DRV_BOTONES

ev_PULSAR_BOTON; 
aux = BUTTON;
void iniciar_botones(EVENTO_T evento, void(*f_callback)(uint32_t id, uint32_t aux));

tratar_boton();

pulsar_boton();

#endif
