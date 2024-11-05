/* *****************************************************************************
	* P.H.2024: Driver/Manejador de los Leds
	* blink practica 2 de proyecto hardware 2024
	* Nekane Diaz Montoya   870795	
	* Jorge Hernandez Aznar 872838
 */
 
#include <stdint.h>
#include <stdbool.h>

#include "hal_gpio.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"
#include "board.h"
#include "rt_fifo.h"

#define RETARDO_MS 5000 		//retardo blink en milisegundos

void leds_c(uint32_t id, uint32_t ms) { //modo RSI
	drv_led_conmutar(id);
}

/* *****************************************************************************
 * BLINK, parpadeo de un led conmutando on/off 
 * retardo por bucle de instrucciones, solo usa el manejador del led
 * para realizar la primera sesión de la practica
 */
void blink_v1(uint32_t id){
	drv_led_encender(id);
  while (true) {
    uint32_t volatile tmo; // declaramos el periodo de tiempo durante el que no se va a producir el cambio
    
    tmo = 10000000;
    while (tmo--); // se decrementa el tiempo
    	drv_led_conmutar(id); // conmuta el led seleccionado por id   
	}		
}

/* *****************************************************************************
 * BLINK, parpadeo de un led conmutando on/off 
 * activacion por tiempo, usa tanto manejador del led como el del tiempo
 * para realizar en la segunda sesión de la practica, version a entregar
 */
void blink_v2(uint32_t id){
	Tiempo_ms_t siguiente_activacion; // declaramos cuando será la siguiente activación
	
	drv_led_encender(id); // encendemos el led seleccionado por id

	siguiente_activacion = drv_tiempo_actual_ms(); // obtenemos el tiempo del instante actual en ms
	
	/* Toggle LEDs. */
	while (true) {
		siguiente_activacion += RETARDO_MS; // la siguiente activación será a los RETARDO_MS ms
		drv_tiempo_esperar_hasta_ms(siguiente_activacion); // se esperará hasta el momento siguiente_activacion
		drv_led_conmutar(id);
	}
}

void blink_v3(uint32_t id){
	drv_consumo_iniciar(MONITOR3, MONITOR1);
	drv_led_encender(id);
	drv_tiempo_periodico_ms(RETARDO_MS, leds_c, id);
	while (true) {
		//__WFI
		drv_consumo_esperar();
	}
}

void blink_v4(uint32_t id){
	EVENTO_T id_evento;
	uint32_t varAux;
	Tiempo_us_t timeStamp;
	
	drv_consumo_iniciar(MONITOR3, MONITOR1);
	rt_FIFO_inicializar(MONITOR4);
	drv_led_encender(id);
	drv_tiempo_periodico_ms(RETARDO_MS, rt_FIFO_encolar, ev_T_PERIODICO);
	while (true) {
		if (rt_FIFO_extraer(&id_evento, &varAux, &timeStamp)) {
			if(id_evento == ev_T_PERIODICO){
				drv_led_conmutar(id);
			}
		}
		else{
			drv_consumo_esperar();
		}
	}
}


/* *****************************************************************************
 * MAIN, Programa principal.
 * para la primera sesion se debe usar la funcion de blink_v1 sin temporizadores
 * para la entrega final se debe incocar a blink_v2
 */
int main(void){
	uint32_t Num_Leds; // declaramos el número de leds
	
	hal_gpio_iniciar();	  // llamamos a iniciar gpio antes de que lo hagan los drivers
	drv_tiempo_iniciar(); // iniciamos el tiempo del driver

	/* Configure LED */
	Num_Leds = drv_leds_iniciar(); // iniciamos los leds
	
	//verificar_over_flow_cola(MONITOR4);
	
	int resultado = verificar_insertar_extraer_en_cola(MONITOR4);
	
	if (Num_Leds > 0){ 
			//blink_v1(1);			 // sesion 1 de practica 2
			//blink_v2(2);			// sesion 2 de practica 2
			blink_v3(3);		 // sesion 1 de practica 3
			//blink_v4(4); 		 	// sesion 2 de practica 3
	}
}
