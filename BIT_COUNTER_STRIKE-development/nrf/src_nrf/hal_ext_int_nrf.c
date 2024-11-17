#include "hal_ext_int.h"
#include <nrf.h>
#include "rt_fifo.h"


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
					
	rt_FIFO_encolar(ev_BOTON_RETARDO, ID_BOTON);
}

void hal_ext_int_iniciar(){
	NVIC_EnableIRQ(GPIOTE_IRQn);
}


void hal_habilitar_int(uint32_t pin){
	if(pin >= 0 && pin < 32){
		NRF_GPIOTE->CONFIG[pin] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
                          		  (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) |
                          		  (pin << GPIOTE_CONFIG_PSEL_Pos);
		NRF_GPIOTE->INTENSET = (1 << pin);
	}
}

void hal_deshabilitar_int(uint32_t pin){
	NVIC_DisableIRQ(GPIOTE_IRQn);
}

void hal_habilitar_despertar(){
	NRF_POWER->TASKS_LOWPWR = 1;
}

void hal_deshabilitar_despertar(){
	NRF_POWER->TASKS_LOWPWR = 0;
}


