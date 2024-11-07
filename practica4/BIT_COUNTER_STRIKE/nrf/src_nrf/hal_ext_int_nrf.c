#include "hal_ext_int.h"
#include <nrf.h>

void hal_ext_int_iniciar(){
		for(int pin = 0; pin < 32; pin++){
				NRF_GPIO->PIN_CNF[pin]  = (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
		}
}

void hal_habilitar_int(uint32_t pin){
		if( pin < 32){
				NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);
		}
}

void hal_deshabilitar_int(uint32_t pin){
		if( pin < 32){
				NRF_GPIO->PIN_CNF[pin] = ~(GPIO_PIN_CNF_SENSE_Msk);
		}
}

void hal_habilitar_despertar(){
		NRF_POWER->TASKS_LOWPWR = 1;
}

void hal_deshabilitar_despertar(){
		NRF_POWER->TASKS_LOWPWR = 0;
}