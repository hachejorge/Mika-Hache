/* *****************************************************************************
	* P.H.2024: main.c
	* blink practica 2 de proyecto hardware 2024
	* Nekane Diaz Montoya   870795	
	* Jorge Hernandez Aznar 872838
 */

#include <stdint.h>
#include <stdbool.h>

#include "hal_gpio.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "Counter_strike.h"
#include "test.h"

#define TEST_MODE 0
#define TEST 1

/* *****************************************************************************
 * MAIN, Programa principal.
 */
 int main(void){
	
	hal_gpio_iniciar();	  // llamamos a iniciar gpio antes de que lo hagan los drivers
	drv_tiempo_iniciar();	// inciamos el tiempo
	drv_leds_iniciar(); // iniciamos los leds
	
		 
	#if TEST_MODE
	 switch (TEST){
	 case 1:
		test_WDT();
	 break;
	 case 2:
	 	pruebas_botones();
	 break;
	 case 3:
		test_alarmas();
	 break;
	 case 4:
		test_rt_FIFO();
	 break;
	 }
	#else
		iniciar_juego();
	#endif
	 
}


