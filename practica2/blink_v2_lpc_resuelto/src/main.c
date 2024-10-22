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

#define RETARDO_MS 500 		//retardo blink en milisegundos

/* *****************************************************************************
 * BLINK, parpadeo de un led conmutando on/off 
 * retardo por bucle de instrucciones, solo usa el manejador del led
 * para realizar la primera sesi�n de la practica
 */
void blink_v1(uint32_t id){
  while (1) {
    uint32_t volatile tmo; // declaramos el periodo de tiempo durante el que no se va a producir el cambio
    
    tmo = 10000000;
    while (tmo--); // se decrementa el tiempo
    drv_led_conmutar(id); // conmuta el led seleccionado por id   
	}		
}

/* *****************************************************************************
 * BLINK, parpadeo de un led conmutando on/off 
 * activacion por tiempo, usa tanto manejador del led como el del tiempo
 * para realizar en la segunda sesi�n de la practica, version a entregar
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
		//otras cosas
	}
}

/* *****************************************************************************
 * MAIN, Programa principal.
 * para la primera sesion se debe usar la funcion de blink_v1 sin temporizadores
 * para la entrega final se debe incocar a blink_v2
 */
int main(void){
	uint32_t Num_Leds; // declaramos el número de leds

	/* Init tiempo, es un reloj que indica el tiempo desde que comenzo la ejecuci�n */
//	drv_tiempo_iniciar(); // para la sesion 2 de practica 2
	
	hal_gpio_iniciar();	// llamamos a iniciar gpio antesde que lo hagan los drivers
	drv_tiempo_iniciar(); // iniciamos el tiempo del driver

	/* Configure LED */
	Num_Leds = drv_leds_iniciar(); // iniciamos los leds
	
	if (Num_Leds > 0){ 
		drv_led_encender(1);
		//blink_v1(1);			// sesion 1 de practica 2
		blink_v2(2);			// para la sesion 2 de practica 2
	}
}
