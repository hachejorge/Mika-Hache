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
		pos_led = 0;
		drv_led_encender(leds_juego[pos_led]);
		estado_juego = juego_activo;
		break;
	case juego_activo:
		if(aux == leds_juego[pos_led]){
			drv_led_apagar(leds_juego[pos_led]);
			pos_led = (pos_led + 1) % TAM_LEDS_JUEGO;
			drv_led_encender(leds_juego[pos_led]);
		}		
		else{
			drv_led_apagar(leds_juego[pos_led]);
			pos_led = 0;
			estado_juego = juego_reposo;
			rt_FIFO_encolar(ev_INICIAR_JUEGO,0);
		}
		break;
	
	default:
		break;
	}
}

void iniciar_juego(){
	drv_consumo_iniciar(3,4);
	rt_FIFO_inicializar(2);
	rt_GE_iniciar(1);
	svc_alarma_iniciar(1,rt_FIFO_encolar,ev_T_PERIODICO);
	drv_botones_iniciar(rt_FIFO_encolar,ev_PULSAR_BOTON, ev_BOTON_RETARDO);
	svc_GE_suscribir(ev_INICIAR_JUEGO, juego);
	svc_GE_suscribir(ev_PULSAR_BOTON, juego);
	estado_juego = juego_reposo;
	rt_FIFO_encolar(ev_INICIAR_JUEGO,0);
	rt_GE_lanzador();
}

