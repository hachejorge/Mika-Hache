/* *****************************************************************************
  * P.H.2024: Driver/Manejador de los Leds
	*
	* blink practica 2 de proyecto hardware 2024
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
 * para realizar la primera sesión de la practica
 */
void blink_v1(uint32_t id){
  while (1) {
    uint32_t volatile tmo;
    
    tmo = 10000000;
    while (tmo--);
    drv_led_conmutar(id);     
	}		
}

/* *****************************************************************************
 * BLINK, parpadeo de un led conmutando on/off 
 * activacion por tiempo, usa tanto manejador del led como el del tiempo
 * para realizar en la segunda sesión de la practica, version a entregar
 */
void blink_v2(uint32_t id){
	Tiempo_ms_t siguiente_activacion;	
	
	drv_led_encender(id);

	siguiente_activacion = drv_tiempo_actual_ms();
	
	/* Toggle LEDs. */
	while (true) {
		siguiente_activacion += RETARDO_MS; //ms
		drv_tiempo_esperar_hasta_ms(siguiente_activacion);
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
	uint32_t Num_Leds;

	/* Init tiempo, es un reloj que indica el tiempo desde que comenzo la ejecución */
//	drv_tiempo_iniciar(); // para la sesion 2 de practica 2
	
	hal_gpio_iniciar();	// llamamos a iniciar gpio antesde que lo hagan los drivers
	drv_tiempo_iniciar();

	/* Configure LED */
	Num_Leds = drv_leds_iniciar();
	
	if (Num_Leds > 0){
		drv_led_encender(1);
		//blink_v1(1);			// sesion 1 de practica 2
		blink_v2(2);			// para la sesion 2 de practica 2
	}
}
