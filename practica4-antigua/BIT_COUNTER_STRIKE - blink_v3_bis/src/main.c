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
#include "rt_FIFO.h"
#include "drv_botones_ini.h"
#include "rt_GE.h"



#define RETARDO_MS 500 		//retardo blink en milisegundos

void leds_c(uint32_t id, uint32_t ms){
		drv_led_conmutar(id);
}

void blink_v3_bis(uint32_t id){
	drv_consumo_iniciar(MONITOR1, MONITOR2);
	drv_led_apagar(id);
	test_gpiote_button_int();
	drv_tiempo_periodico_ms(RETARDO_MS, leds_c, id);
	while(true){
		for(int i = 0; i < 20; i++){
			drv_consumo_esperar();
		}
		drv_consumo_dormir();
	}
}

void comprobar_ruido_boton(){
	rt_FIFO_inicializar(MONITOR1);
	test_gpiote_button_int();
		drv_consumo_iniciar(MONITOR1, MONITOR2);
	//drv_consumo_esperar();
		
	rt_GE_iniciar(MONITOR3);
	rt_GE_lanzador();
}


/* *****************************************************************************
 * MAIN, Programa principal.
 * para la primera sesion se debe usar la funcion de blink_v1 sin temporizadores
 * para la entrega final se debe incocar a blink_v2
 */
int main(void){
	uint32_t Num_Leds; // declaramos el número de leds
	
	hal_gpio_iniciar();	  // llamamos a iniciar gpio antes de que lo hagan los drivers
	drv_tiempo_iniciar();
	hal_gpio_sentido(BUTTON_3,HAL_GPIO_PIN_DIR_INPUT);
	/* Configure LED */
	Num_Leds = drv_leds_iniciar(); // iniciamos los leds
	comprobar_ruido_boton();
	if (Num_Leds > 0){ 
		//blink_v3_bis(3);
	}
}
