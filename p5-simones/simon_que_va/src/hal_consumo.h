
#ifndef HAL_CONSUMO
#define HAL_CONSUMO
 
//#include "nrf.h"

#include <stdint.h>

void hal_consumo_iniciar(void);
void hal_consumo_esperar(void); //WFI e idle
void hal_consumo_dormir(void);  //aun no en nRF

#endif

