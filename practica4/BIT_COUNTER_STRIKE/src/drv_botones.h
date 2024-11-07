/* *****************************************************************************
 * P.H.2024: hal_gpio_nrf.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */


#include <stdbool.h>
#include <stdint.h> 
#include "rt_evento_t.h"

#ifndef DRV_BOTONES
#define DRV_BOTONES

typedef struct {
    int id;                     // Identificador del bot�n
    bool pulsado;               // Estado del bot�n (true si est� pulsado)
    int estado_actual;          // Estado de la automara en el que se encuentra el bot�n
    int tiempo_rebote;          // Tiempo de retardo para ignorar rebotes
    void (*callback)(int id);   // Callback a ejecutar al presionar el bot�n
} BUTTON;


void iniciar_botones(EVENTO_T evento, void(*f_callback)(uint32_t id, uint32_t aux));


void tratar_boton(uint32_t id);

pulsar_boton();

#endif
