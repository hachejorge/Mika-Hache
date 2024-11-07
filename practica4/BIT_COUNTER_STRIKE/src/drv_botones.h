/* *****************************************************************************
 * P.H.2024: hal_gpio_nrf.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */


#include <stdbool.h>
#include <stdint.h> 
#include "rt_evento_t.h"
#include "rt_fifo.h"


#ifndef DRV_BOTONES
#define DRV_BOTONES

typedef struct {
    int id;                     // Identificador del boton
    bool pulsado;               // Estado del boton (true si est� pulsado)
    int estado_actual;          // Estado de la automara en el que se encuentra el boton
    int tiempo_rebote;          // Tiempo de retardo para ignorar rebotes
    void (*callback)(EVENTO_T id, uint32_t aux);   // Callback a ejecutar al presionar el boton
} BUTTON;

void drv_botones_iniciar(void(*callback)(EVENTO_T id, uint32_t aux), EVENTO_T ev_PULSAR_BOTON, EVENTO_T ev_BOTON_RETARDO);

void drv_botones_tratar(EVENTO_T evento, uint32_t auxiliar);

#endif
