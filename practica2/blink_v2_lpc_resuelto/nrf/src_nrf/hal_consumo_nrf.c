#include <nrf.h>
#include "hal_consumo.h"

void hal_consumo_iniciar(){
    // No es necesario incializar nada 
}

void hal_consumo_esperar(){ 
    __WFI(); // El procesador se "duerme" (modo de bajo consumo) hasta que llega una Interrupción 
}

void hal_consumo_dormir();