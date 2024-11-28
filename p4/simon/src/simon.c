 /* *****************************************************************************
	* P.H.2024: simon.c
	* blink practica 2 de proyecto hardware 2024
	* Nekane Diaz Montoya   870795	
	* Jorge Hernandez Aznar 872838
 */
#include "simon.h"
#include <stdbool.h>

#include "hal_gpio.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"
#include "rt_GE.h"
#include "drv_botones.h"
#include "srv_alarm.h"
#include "rt_FIFO.h"
#include "hal_WDT.h"
#include "board.h"

 enum estados_simon{
	 e_INIT,
	 /*
	 e_SHOW_SEQUENCE,
	 e_WAIT_FOR_INPUT,
	 e_SUCCESS,
	 e_FAIL,
	 */
	
} estado_simon;
 
void leds_conmutar(){
		for(int i = 0; i < LEDS_NUMBER; i++){
					drv_led_conmutar(i);
		}
}


void simon_iniciar(){
	drv_consumo_iniciar(3,4);
	rt_FIFO_inicializar(2);
	rt_GE_iniciar(1);
	svc_alarma_iniciar(1,rt_FIFO_encolar,ev_T_PERIODICO);
	drv_botones_iniciar(rt_FIFO_encolar,ev_PULSAR_BOTON, ev_BOTON_RETARDO);
	estado_simon = e_INIT;
	rt_FIFO_encolar(ev_INICIAR_JUEGO,0);
	svc_GE_suscribir(ev_INICIAR_JUEGO, simon_tratar);
	rt_GE_lanzador();
}

void simon_tratar(EVENTO_T evento, uint32_t aux){
	switch(estado_simon) 
	{
		case e_INIT:
			if(evento == ev_INICIAR_JUEGO){
				svc_alarma_activar(false, 2000, ev_LEDS_ENCENDER, 0);
				svc_GE_suscribir(ev_LEDS_ENCENDER, leds_conmutar);
				svc_alarma_activar(false, 4000, ev_LEDS_APAGAR, 0);
				svc_GE_suscribir(ev_LEDS_APAGAR, leds_conmutar);
				
			}
			break;
			/*
		case e_SHOW_SEQUENCE:
			
			break;
		case e_WAIT_FOR_INPUT:
			
			break;
		case e_SUCCESS:
			
			break;
		case e_FAIL:
			
			break;
			*/
	}		
}
