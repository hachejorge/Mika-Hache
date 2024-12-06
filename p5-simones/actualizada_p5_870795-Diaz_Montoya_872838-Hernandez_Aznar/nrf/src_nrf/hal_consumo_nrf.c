/* *****************************************************************************
 * P.H.2024: hal_consumo_nrf.c
 * implementación del módulo hal_consumo para la placa nrf52840
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */

#include <nrf.h>
#include "hal_consumo.h"

void hal_consumo_iniciar(){
    // No es necesario incializar nada 
}

void hal_consumo_esperar(){ 
    __WFI(); // El procesador se "duerme" (modo de bajo consumo) hasta que llega una Interrupción 
}

void hal_consumo_dormir(){
	// Se apaga el procesador
	NRF_POWER->SYSTEMOFF = POWER_SYSTEMOFF_SYSTEMOFF_Enter;
	
	// Se espera hasta que llegue un evento
	while(1){
		__WFE();
	}
}

