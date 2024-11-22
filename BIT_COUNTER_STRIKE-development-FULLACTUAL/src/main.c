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
#include "rt_GE.h"
#include "drv_botones_lpc.h"
#include "drv_botones.h"
#include "srv_alarm.h"
#include "rt_FIFO.h"

#define RETARDO_MS 500 		//retardo blink en milisegundos

#define TEST_MODE 1

void test_rt_FIFO(){
	rt_FIFO_inicializar(1);
	//verificar_insertar_extraer_en_cola();
	verificar_overflow_cola();
}

void leds_c(EVENTO_T evento, uint32_t id){
	drv_led_conmutar(id);
}
/*
void blink_v3_bis(uint32_t id){
	drv_consumo_iniciar(MONITOR1, MONITOR2);
	drv_led_apagar(id);
	init_eint0_button();
	//drv_tiempo_periodico_ms(RETARDO_MS, leds_c, id);
	rt_GE_iniciar(MONITOR3);
	rt_GE_lanzador();
}

void pruebas_alarma_GE(){
	rt_GE_iniciar(MONITOR3);
	//svc_alarma_iniciar(MONITOR1,rt_FIFO_encolar,ev_T_PERIODICO);
	init_eint0_button();
	//svc_alarma_activar(svc_alarma_codificar(1, 3000), ev_PULSAR_BOTON, 3);
	svc_GE_suscribir(ev_PULSAR_BOTON, leds_c);
	rt_GE_lanzador();
}
*/
void pruebas_botones(){
	drv_botones_iniciar(rt_FIFO_encolar,ev_PULSAR_BOTON, ev_BOTON_RETARDO);
	svc_alarma_iniciar(1,rt_FIFO_encolar,ev_T_PERIODICO);
	svc_GE_suscribir(ev_PULSAR_BOTON,leds_c);
	rt_GE_iniciar(1);
	rt_GE_lanzador();
}

/* *****************************************************************************
 * MAIN, Programa principal.
 * para la primera sesion se debe usar la funcion de blink_v1 sin temporizadores
 * para la entrega final se debe incocar a blink_v2
 */
 int main(void){
	
	hal_gpio_iniciar();	  // llamamos a iniciar gpio antes de que lo hagan los drivers
	drv_tiempo_iniciar();
	/* Configure LED */
	uint32_t Num_Leds; // declaramos el número de leds
	Num_Leds = drv_leds_iniciar(); // iniciamos los leds
	
	#if TEST_MODE
		pruebas_botones();
		//pruebas_alarma_GE();
		//test_rt_FIFO();
	#else

	if (Num_Leds > 0){ 
		//blink_v3_bis(3);
	}
	#endif
}
