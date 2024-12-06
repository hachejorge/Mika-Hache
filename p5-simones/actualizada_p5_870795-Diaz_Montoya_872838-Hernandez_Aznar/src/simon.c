 /* *****************************************************************************
	* P.H.2024: simon.c
	* blink practica 2 de proyecto hardware 2024
	* Nekane Diaz Montoya   870795	
	* Jorge Hernandez Aznar 872838
 */
#include "simon.h"
#include <stdbool.h>

#include "drv_leds.h"
#include "drv_tiempo.h"
#include "rt_GE.h"
#include "drv_botones.h"
#include "srv_alarm.h"
#include "rt_FIFO.h"
#include "hal_random.h"
#include "board.h"

// tamaño del vector de leds del simon 
#define TAM_SIMON 20 
// velocidad inicial del simon
#define VELOCIDAD 20	

static bool primera_vez = true; // Bandera para controlar si es la primera ejecución

 enum estados_simon{
	 e_INIT,
	 e_SHOW_SEQUENCE,
	 e_WAIT_FOR_INPUT,
	 e_SUCCESS,
	 e_FAIL,
} estado_simon;

// Leds de la secuencia del simon
static int leds_simon[TAM_SIMON];

// posicion a alacanzar del vector de leds en el juego general
static int posicion_simon;
// posición alcanzada del vector de leds
static int posicion_pulsada;
static int velocidad;

// conmuta el led de on a off y viceversa, primero consulta el estado y lo invierte
void leds_conmutar(EVENTO_T evento, uint32_t aux){
		drv_led_conmutar(aux);
}

//muestra la secuencia inicial de luces de manera secuencial, encendiendo y apagando los LEDs de forma alternada
void simon_mostrar_seq_inicial(EVENTO_T evento, uint32_t aux){
		if(aux < LEDS_NUMBER * 2){ // mientras que hayan leds por procesar
			leds_conmutar(ev_VOID, aux/2 + 1); // se alterna el estado del led correspondiente
			svc_alarma_activar(svc_alarma_codificar(0,250), ev_JUEGO_SEQ_INCIAL, aux + 1); // reprograma la misma función con aux++
		}
		else{
			svc_alarma_activar(svc_alarma_codificar(0,250), ev_JUEGO_CONTINUAR,0); // se programa una alarma para avanzar al siguiente estado de juego
		}
}

// muestra la secuencia del juego de luces de manera secuencial, encendiendo y apagando los LEDs de forma alternada
void simon_mostrar_seq_juego(EVENTO_T evento, uint32_t aux){
		if(aux < posicion_simon * 2){ // mientras que hayan leds por procesar hasta esa partida
			leds_conmutar(ev_VOID, leds_simon[aux/2]); // se alterna el estado del led correspondiente
			svc_alarma_activar(svc_alarma_codificar(0, 20 * velocidad), ev_JUEGO_SEQ_JUEGO, aux + 1); // reprograma la misma función con aux++
		}
		else{
			estado_simon = e_WAIT_FOR_INPUT;
			svc_alarma_activar(svc_alarma_codificar(0, 5000), ev_TIEMPO_SUPERADO, 0); // se programa una alarma para avanzar al siguiente estado de juego
		}
}

// Reiniciamos el juego volviendo al estado inicial del juego en la  máquina de estados y al primer elemento del vector de leds del simon
void reiniciar_juego(EVENTO_T evento, uint32_t aux){
		estado_simon = e_INIT; // colocamos el valor de la maquina de estados en la posicón inicial
		posicion_simon = 1;
	
		svc_alarma_activar(svc_alarma_codificar(0,10),ev_INICIAR_JUEGO,0);	// se programa una alarma para avanzar al siguiente estado de juego (INICIAR_JUEGO)
}

// para acierto, se encienden todos los leds
void simon_conmutar_leds(EVENTO_T evento, uint32_t aux){
		if(aux < 2){
			drv_leds_conmutar_todos();
			svc_alarma_activar(svc_alarma_codificar(0,500), ev_MOSTRAR_ACIERTO, aux + 1); // reprograma la misma función con aux++ para apagar el led que está encendido
		}
		else{
			if(posicion_simon == TAM_SIMON + 1){	// en el caso de que la posicion del simon es la máxima posible se reincia el juego
					reiniciar_juego(ev_INICIAR_JUEGO, 0); // se programa una alarma para reiniciar el estado de juego
			}
			else{
				svc_alarma_activar(svc_alarma_codificar(0, 10), ev_JUEGO_CONTINUAR, 0); // en caso de que queden más leds por tratar, se continua el juego
			}
		}
}

// enciende el led y programa una alarma para apagarlo
void encender_led(EVENTO_T evento, uint32_t aux){
		if(estado_simon == e_WAIT_FOR_INPUT) { // Si está esperando realmente el input del botón
				drv_leds_apagar_todos();
				drv_led_encender(aux);
				svc_alarma_activar(svc_alarma_codificar(0, 500), ev_APAGAR_LED, aux); // se programa una alarma para apagar el led encendido
		}
}

// apaga el led
void apagar_led(EVENTO_T evento, uint32_t aux){
		drv_led_apagar(aux);
}

void simon_tratar(EVENTO_T evento, uint32_t aux){
	switch(estado_simon) 
	{
		case e_INIT:
			if(evento == ev_INICIAR_JUEGO){
				 if (primera_vez) { //si es la primera que se incia el juego
               // Secuencia inicial basada en la placa
							if (PRIMERA_PARTIDA == 0) { // nRF
									int secuencia_temporal[TAM_SIMON] = {1, 2, 3, 4, 3, 2, 1, 2, 3, 4, 1, 2, 3, 4, 3, 2, 1, 2, 3, 4};
									for (int i = 0; i < TAM_SIMON; i++) {
										leds_simon[i] = secuencia_temporal[i];
									}
							} else if (PRIMERA_PARTIDA == 1) { // LPC
									int secuencia_temporal[TAM_SIMON] = {1, 2, 3, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 2, 1, 2, 3, 2, 1, 2};
									for (int i = 0; i < TAM_SIMON; i++) {
										leds_simon[i] = secuencia_temporal[i];
									}
							}
							primera_vez = false; // Marca que ya se ejecutó la secuencia inicial
					} else {
							// Secuencia aleatoria después de la primera ejecución
							for (int i = 0; i < TAM_SIMON; i++) {
									leds_simon[i] = hal_random_generar(1, BUTTONS_NUMBER);
							}
					}
				
				for(int i = 1; i <= LEDS_NUMBER; i++){
						drv_led_apagar(i);	//apagamos todos los leds incialmente
				}
				velocidad = VELOCIDAD; // asignamos la velocidad como 20 incialmente
				svc_alarma_activar(svc_alarma_codificar(0, 10), ev_JUEGO_SEQ_INCIAL, 0); // se programa una alarma para que se muestre la secuencia incial del juego

				estado_simon = e_SHOW_SEQUENCE; // avanzamos hasta el siguiente estado del juego simon
			}
			break;

		case e_SHOW_SEQUENCE:
			if(evento == ev_JUEGO_CONTINUAR){
				// una vez mostrada la secuencia incial incializamos a 0 la posicion pulsada
				posicion_pulsada = 0; 

				svc_alarma_activar(svc_alarma_codificar(0, 10), ev_JUEGO_SEQ_JUEGO, 0); // se programa una alarma para que se muestre la secuencia del juego
				
			}
			break;

		case e_WAIT_FOR_INPUT:

			if(ev_PULSAR_BOTON == evento){
				svc_alarma_activar(svc_alarma_codificar(0, 0), ev_TIEMPO_SUPERADO, 0); // se cancela la alarma de reinciar porque se ha pulsado el boton antes de que se supere el tiempo
				if(aux == leds_simon[posicion_pulsada]){ // Bien pulsado
					if(posicion_simon == posicion_pulsada + 1){ // es el último led de la secuencia actual
						estado_simon = e_SUCCESS;
						svc_alarma_activar(svc_alarma_codificar(0, 500), ev_JUEGO_CONTINUAR, 0);  // se programa una alarma para seguir el juego
					}
					else{
						posicion_pulsada++; // aumentamos la posicion en el vector de leds para mostrar el siguiente led
						svc_alarma_activar(svc_alarma_codificar(0, 5000), ev_TIEMPO_SUPERADO, 0); // se programa una alarma que si se superan 5s de inactividad, reincia el juego
					}
				}
				else{ // Fallo 
					estado_simon = e_FAIL;
					svc_alarma_activar(svc_alarma_codificar(0,10),ev_JUEGO_CONTINUAR,0); // se programa una alarma para seguir el juego
				}
			}
				
			break;
		
		case e_SUCCESS:
			if(evento == ev_JUEGO_CONTINUAR){
				drv_leds_apagar_todos();
				posicion_simon++; // aumentamos la posición del vector general
				estado_simon = e_SHOW_SEQUENCE; // avanzamos hasta el estado de mistrar la secuencia para enseñar la siguiente secuencia a pulsar
				velocidad--;		 // reducimos la velocidad para aumentar la dificultad
				svc_alarma_activar(svc_alarma_codificar(0, 500), ev_MOSTRAR_ACIERTO, 0); // se programa una alarma para mostrar que se ha acertado la secuencia
			}
			break;
		
		case e_FAIL:
			if(evento == ev_JUEGO_CONTINUAR){ 
				drv_leds_apagar_todos();
				posicion_simon = 1;	// si se ha fallado volvemos a inciar el vector simon
				estado_simon = e_INIT; // volvemos al estado incial
				svc_alarma_activar(svc_alarma_codificar(0, 500), ev_INICIAR_JUEGO, 0); // se programa una alarma para iniciar el juego otra vez
			}
			break;
			
		default:
			break;
	}		
}

void simon_iniciar(){
	// incializamos el modulo de la FIFO, el GE y random
	rt_FIFO_inicializar(2); 
	rt_GE_iniciar(1);
	hal_random_iniciar(drv_tiempo_actual_ms());
	
	// incializamos el módulo de alarmas y los botones
	svc_alarma_iniciar(1,rt_FIFO_encolar,ev_T_PERIODICO);
	drv_botones_iniciar(rt_FIFO_encolar,ev_PULSAR_BOTON, ev_BOTON_RETARDO);
	
	// suscribimos la función reinciair_juego a ev_TIEMPO_SUPERADO
	svc_GE_suscribir(ev_TIEMPO_SUPERADO, reiniciar_juego);
	
	// suscribimos la función simon_conmutar_leds a ev_MOSTRAR_ACIERTO
	svc_GE_suscribir(ev_MOSTRAR_ACIERTO, simon_conmutar_leds);
	
	// suscribimos la función simon_mostrar_seq_juego a ev_JUEGO_SEQ_JUEGO
	svc_GE_suscribir(ev_JUEGO_SEQ_JUEGO, simon_mostrar_seq_juego);
	// suscribimos la función simon_mostrar_seq_inicial a ev_JUEGO_SEQ_INCIAL
	svc_GE_suscribir(ev_JUEGO_SEQ_INCIAL, simon_mostrar_seq_inicial);
	
	// suscribimos la función encender_led a ev_PULSAR_BOTON
	svc_GE_suscribir(ev_PULSAR_BOTON, encender_led);
	// suscribimos la función simon_tratar a ev_PULSAR_BOTON
	svc_GE_suscribir(ev_PULSAR_BOTON, simon_tratar);
	
	// suscribimos la función simon_tratar a ev_JUEGO_CONTINUAR
	svc_GE_suscribir(ev_JUEGO_CONTINUAR, simon_tratar);
	
	// suscribimos la función simon_tratar a ev_INICIAR_JUEGO
	svc_GE_suscribir(ev_INICIAR_JUEGO, simon_tratar);
	
	// suscribimos la función reiniciar_juego a ev_RESET_BOTON
	svc_GE_suscribir(ev_RESET_BOTON, reiniciar_juego);
	
	// suscribimos la función apagar_led a ev_APAGAR_LED
	svc_GE_suscribir(ev_APAGAR_LED, apagar_led);
	
// se programa una alarma para inciar el juego
	svc_alarma_activar(svc_alarma_codificar(0,100),ev_INICIAR_JUEGO,0);
	// incializamos el estado del juego y la posicion del simon
	estado_simon = e_INIT;
	posicion_simon = 1;
	
	// inicializamos el lanzador del GE
	rt_GE_lanzador();
}

