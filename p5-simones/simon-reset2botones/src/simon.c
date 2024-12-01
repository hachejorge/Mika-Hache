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
#include "hal_random.h"
#include "board.h"

#define TAM_SIMON 20

 enum estados_simon{
	 e_INIT,
	 e_SHOW_SEQUENCE,
	 e_WAIT_FOR_INPUT,
	 e_SUCCESS,
	 e_FAIL,
} estado_simon;

// Leds de la secuencia del simon
static int leds_simon[TAM_SIMON];

static int posicion_simon;
static int posicion_pulsada;
static int velocidad;

void leds_conmutar(EVENTO_T evento, uint32_t aux){
		drv_led_conmutar(aux);
}

void simon_mostrar_seq_inicial(EVENTO_T evento, uint32_t aux){
		if(aux < LEDS_NUMBER * 2){
			leds_conmutar(ev_VOID, aux/2 + 1);
			svc_alarma_activar(svc_alarma_codificar(0,250), ev_JUEGO_SEQ_INCIAL, aux + 1);
		}
		else{
			svc_alarma_activar(svc_alarma_codificar(0,250), ev_JUEGO_CONTINUAR,0);
		}
}

void simon_mostrar_seq_juego(EVENTO_T evento, uint32_t aux){
		if(aux < posicion_simon * 2){
			leds_conmutar(ev_VOID, leds_simon[aux/2]);
			svc_alarma_activar(svc_alarma_codificar(0, 20 * velocidad), ev_JUEGO_SEQ_JUEGO, aux + 1);
		}
		else{
			estado_simon = e_WAIT_FOR_INPUT;
			svc_alarma_activar(svc_alarma_codificar(0, 5000), ev_TIEMPO_SUPERADO, 0);
		}
}

//Para acierto, se encienden todos
void simon_conmutar_leds(EVENTO_T evento, uint32_t aux){
		if(aux < 2){
			for(int i = 1; i <= LEDS_NUMBER; i++){
						drv_led_conmutar(i);
			}
			svc_alarma_activar(svc_alarma_codificar(0,500), ev_LEDS_ENCENDER, aux + 1);
		}
		else{
			svc_alarma_activar(svc_alarma_codificar(0, 10), ev_JUEGO_CONTINUAR, 0);
		}
}

void reiniciar_juego(EVENTO_T evento, uint32_t aux){
		estado_simon = e_INIT;
		posicion_simon = 1;
		
		svc_alarma_activar(svc_alarma_codificar(0, 0), ev_JUEGO_SEQ_JUEGO, 0);
		svc_alarma_activar(svc_alarma_codificar(0,0), ev_JUEGO_SEQ_INCIAL, 0);
	
	
		svc_alarma_activar(svc_alarma_codificar(0,10),ev_INICIAR_JUEGO,0);	
}


void simon_iniciar(){
	drv_consumo_iniciar(3,4);
	rt_FIFO_inicializar(2);
	rt_GE_iniciar(1);
	hal_random_iniciar(drv_tiempo_actual_ms());
		
	svc_alarma_iniciar(1,rt_FIFO_encolar,ev_T_PERIODICO);
	drv_botones_iniciar(rt_FIFO_encolar,ev_PULSAR_BOTON, ev_BOTON_RETARDO);
	
	svc_GE_suscribir(ev_TIEMPO_SUPERADO, reiniciar_juego);
	svc_GE_suscribir(ev_LEDS_ENCENDER, simon_conmutar_leds);
	
	svc_GE_suscribir(ev_JUEGO_SEQ_JUEGO, simon_mostrar_seq_juego);
	svc_GE_suscribir(ev_JUEGO_SEQ_INCIAL, simon_mostrar_seq_inicial);
	
	svc_GE_suscribir(ev_PULSAR_BOTON, simon_tratar);
	
	svc_GE_suscribir(ev_JUEGO_CONTINUAR, simon_tratar);
	svc_GE_suscribir(ev_INICIAR_JUEGO, simon_tratar);
	
	svc_GE_suscribir(ev_RESET_BOTON, reiniciar_juego);
	
	svc_alarma_activar(svc_alarma_codificar(0,100),ev_INICIAR_JUEGO,0);
	
	estado_simon = e_INIT;
	posicion_simon = 1;
	
	rt_GE_lanzador();
}




void simon_tratar(EVENTO_T evento, uint32_t aux){
	switch(estado_simon) 
	{
		case e_INIT:
			if(evento == ev_INICIAR_JUEGO){
				// Iría en el main??
				for(int i = 0; i <TAM_SIMON; i++){
					leds_simon[i] = hal_random_generar(1,BUTTONS_NUMBER);
				}
				for(int i = 1; i <= LEDS_NUMBER; i++){
						drv_led_apagar(i);
				}
				velocidad = 20;
				svc_alarma_activar(svc_alarma_codificar(0, 10), ev_JUEGO_SEQ_INCIAL, 0);				

				estado_simon = e_SHOW_SEQUENCE;
			}
			break;

		case e_SHOW_SEQUENCE:
			if(evento == ev_JUEGO_CONTINUAR){
				posicion_pulsada = 0;

				svc_alarma_activar(svc_alarma_codificar(0, 10), ev_JUEGO_SEQ_JUEGO, 0);
				
			}
			break;

		case e_WAIT_FOR_INPUT:
				if(ev_PULSAR_BOTON == evento){
						if(aux == leds_simon[posicion_pulsada]){ // Bien pulsado
								svc_alarma_activar(svc_alarma_codificar(0, 5000), ev_TIEMPO_SUPERADO, 0);
								if(posicion_simon == posicion_pulsada + 1){ // Ha pulsado todos bien
										estado_simon = e_SUCCESS;
										svc_alarma_activar(svc_alarma_codificar(0, 500), ev_JUEGO_CONTINUAR, 0);
								}
								else{
										posicion_pulsada++;
								}
						}
						else{ // Fallo 
								estado_simon = e_FAIL;
								svc_alarma_activar(svc_alarma_codificar(0, 0), ev_TIEMPO_SUPERADO, 0);
								svc_alarma_activar(svc_alarma_codificar(0,10),ev_JUEGO_CONTINUAR,0);
						}
				}
				
			break;
		
		case e_SUCCESS:
			if(evento == ev_JUEGO_CONTINUAR){
				posicion_simon++;
				estado_simon = e_SHOW_SEQUENCE;
				velocidad--;
				svc_alarma_activar(svc_alarma_codificar(0, 500), ev_LEDS_ENCENDER, 0);
			}
			break;
		
		case e_FAIL:
			if(evento == ev_JUEGO_CONTINUAR){
				posicion_simon = 1;
				estado_simon = e_INIT;
				svc_alarma_activar(svc_alarma_codificar(0, 500), ev_INICIAR_JUEGO, 0);
			}
			break;
			
		default:
			break;
	}		
}
