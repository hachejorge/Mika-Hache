
#include <nrf.h>
#include <board.h>
#include "drv_botones_ini.h"
#include "rt_FIFO.h"

/* *****************************************************************************
 * el pulsador genera un evento al cambiar de estado (toggle)
 * y genera una interrupcion de tipo: GPIOTE_IRQn
 * la RSI es GPIOTE_IRQHandler, definida en: arm_startup...
 */
void test_gpiote_button_int(){
  // Configure BUTTON3 to generate EVENTS_COMPARE[0] every 1000us.
  NRF_GPIOTE->CONFIG[3] = (GPIOTE_CONFIG_MODE_Event      << GPIOTE_CONFIG_MODE_Pos) |
                          (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) |
                          (BUTTON_3                << GPIOTE_CONFIG_PSEL_Pos);
	
	NRF_GPIOTE->INTENSET = 8;
	
	NVIC_EnableIRQ(GPIOTE_IRQn);
}

void GPIOTE_IRQHandler(){
  volatile uint32_t dummy;
	
	NRF_GPIOTE->EVENTS_IN[3] = 0; //puede tardar 5 ciclos... //ojo con optimizaciones

	dummy = NRF_GPIOTE->EVENTS_IN[3];
  dummy; 
					
	rt_FIFO_encolar(ev_PULSAR_BOTON,3);
	
}

