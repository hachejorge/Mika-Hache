/* *****************************************************************************
	* P.H.2024: Counter_strike.h
	* blink practica 2 de proyecto hardware 2024
	* Nekane Diaz Montoya   870795	
	* Jorge Hernandez Aznar 872838
 */

#ifndef COUNTER_STRIKE
#define COUNTER_STRIKE

#include <stdint.h>
#include "rt_evento_t.h"

/*
 * Inicializa todos los módulos necesarios (drv_consumo, rt_FIFO, rt_GE...) para ejecutar el juego counter strike
*/
void iniciar_juego(void);

/*
 * Contiene la máquina de estados que se realiza el counter strike
*/
void juego(EVENTO_T evento, uint32_t aux);

#endif
