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
#include "drv_botones.h"
#include "srv_alarm.h"
#include "rt_GE.h"
#include "rt_evento_t.h"
#include "board.h"
#include "drv_botones.h"
#include "rt_fifo.h"
#include "hal_ext_int.h"


#define RETARDO_MS 5000 		//retardo blink en milisegundos

#if LEDS_NUMBER > 0
	static uint32_t led_list[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif

void probar_activar_led_con_boton(uint32_t id_led, uint32_t id_boton){
	// Driver de tiempo inicializado
	rt_FIFO_inicializar(MONITOR3);

	svc_alarma_iniciar(MONITOR1, rt_FIFO_encolar, ev_T_PERIODICO);
	rt_GE_iniciar(MONITOR2);

	drv_botones_iniciar(rt_FIFO_encolar, ev_PULSAR_BOTON, ev_BOTON_RETARDO);

	rt_GE_lanzador();

	drv_consumo_dormir();
}

void blink_v3_bis(uint32_t id){
    EVENTO_T evento;
    uint32_t aux;
    Tiempo_us_t ts;

    while(true) {
        if(rt_FIFO_extraer(&evento, &aux, &ts)){ // Extrae pulsar botón
            for(int i = 0; i < 10; i++){
									for(uint32_t led_id = 0; led_id < LEDS_NUMBER; led_id++){                
											drv_led_encender(led_list[led_id]);
											drv_tiempo_esperar_hasta_ms(500);
											drv_led_apagar(led_list[led_id]);
									}
							}
            // Apagar todos los LEDs después de 10 parpadeos
            for(uint32_t led_id = 0; led_id < LEDS_NUMBER; led_id++){
                drv_led_apagar(led_list[led_id]);
            }
        } else {
            drv_consumo_dormir();
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
	drv_tiempo_iniciar();

	/* Configure LED */
	Num_Leds = drv_leds_iniciar(); // iniciamos los leds
	
	drv_consumo_iniciar(MONITOR3, MONITOR1);
	rt_FIFO_inicializar(MONITOR4);
	svc_alarma_iniciar(MONITOR1, rt_FIFO_encolar, ev_T_PERIODICO);
	//rt_GE_iniciar(MONITOR2);
	drv_botones_iniciar(rt_FIFO_encolar, ev_PULSAR_BOTON, ev_BOTON_RETARDO);

	
	if (Num_Leds > 0){ 
		blink_v3_bis(3);
	}
}
