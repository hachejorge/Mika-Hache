/* *****************************************************************************
	* P.H.2024: test.h
	* blink practica 2 de proyecto hardware 2024
	* Nekane Diaz Montoya   870795	
	* Jorge Hernandez Aznar 872838
 */
 
#ifndef TESTS
#define TESTS

#include <stdint.h>
#include <stdbool.h>
#include "rt_evento_t.h"

/*
 * Test que comprueba el funcionamiento del watchdog
*/
void test_WDT(void);

/*
 * Produce un overflow en la fifo
*/
void test_rt_FIFO(void);

/*
 * Conmuta el led introducido, encendiendolo y apagandolo
*/
void leds_c(EVENTO_T evento, uint32_t id);

/*
 * Test que comprueba las alarmas al encolar el ev_PULSAR_BOTON y hacer cada vez que se extraiga este evento que los leds conmuten
*/
void test_alarmas(void);

/*
 * Cada vez que pulsamos un boton, se conmuta el led pulsado  
*/
void pruebas_botones(void);

#endif
