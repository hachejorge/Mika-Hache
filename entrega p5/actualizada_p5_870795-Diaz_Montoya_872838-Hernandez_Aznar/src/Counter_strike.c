/* *****************************************************************************
	* P.H.2024: Counter_strike.c
	* blink practica 2 de proyecto hardware 2024
	* Nekane Diaz Montoya   870795	
	* Jorge Hernandez Aznar 872838
 */


#include <stdbool.h>

#include "hal_gpio.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"
#include "board.h"
#include "rt_GE.h"
#include "drv_botones.h"
#include "srv_alarm.h"
#include "rt_FIFO.h"
#include "hal_WDT.h"
#include "simon.h"

// Estados posibles del flujo habitual de un botón
enum estados_juego { 
	juego_reposo,
	juego_activo
} estado_juego;

static int pos_led = 0;


void juego(EVENTO_T evento, uint32_t aux){
	switch (estado_juego)
	{
	case juego_reposo:
		pos_led = 0; // se pone el indice del vector en el primer elemento 
		drv_led_encender(leds_juego[pos_led]); // encendemos los leds que indica el vector leds_juego para mostrar la secuencia
		estado_juego = juego_activo; // cambiamos el estado del juego a juego_activo
		break;
	case juego_activo:
		if(aux == leds_juego[pos_led]){ // si el boton es igual al led encendido
			drv_led_apagar(leds_juego[pos_led]); // apagamos el led
			pos_led = (pos_led + 1) % TAM_LEDS_JUEGO; // aumentamos el led a encender en el vector leds_juego
			drv_led_encender(leds_juego[pos_led]);	// encendemos el siguiente led a pulsar
		}		
		else{
			drv_led_apagar(leds_juego[pos_led]); //// si el boton no es igual al led encendido, apagamos el led
			pos_led = 0; // reiniciamos a la posicion incial del vector
			estado_juego = juego_reposo;
			rt_FIFO_encolar(ev_INICIAR_JUEGO,0); // volvemos a encolar el evento para inciar el juego
		}
		break;
	
	default:
		break;
	}
}

void iniciar_juego(){
	drv_consumo_iniciar(3,4); // iniciamos el drv_comsumo
	rt_FIFO_inicializar(2); 	// iniciamos la fifo
	rt_GE_iniciar(1);					// iniciamos el gestor de eventos
	svc_alarma_iniciar(1,rt_FIFO_encolar,ev_T_PERIODICO); // inciamos la alarma periódica ev_T_PERIODICO que se encolará en la fifo cada segundo
	drv_botones_iniciar(rt_FIFO_encolar,ev_PULSAR_BOTON, ev_BOTON_RETARDO); // inciamos los botones 
	svc_GE_suscribir(ev_INICIAR_JUEGO, juego); // se sucribe juego al ev_INICIAR_JUEGO
	svc_GE_suscribir(ev_PULSAR_BOTON, juego);	 // se suscribe juego a ev_PULSAR_BOTON
	estado_juego = juego_reposo;							 // se inicializa el estado del juego a juego reposo
	rt_FIFO_encolar(ev_INICIAR_JUEGO,0);			 // se encola inicialmente el ev_INICIAR_JUEGO
	rt_GE_lanzador();													 // se inicializa el gestor de eventos
}

