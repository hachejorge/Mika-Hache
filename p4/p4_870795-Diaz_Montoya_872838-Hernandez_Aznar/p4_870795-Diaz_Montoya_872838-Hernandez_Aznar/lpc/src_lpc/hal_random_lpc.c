/* *****************************************************************************
 * P.H.2024: hal_random_lpc.c
 * Nekane Diaz Montoya   870795
 * Jorge Hernandez Aznar 872838
 */

#include "hal_random.h"
#include <stdlib.h>

void hal_random_iniciar(uint32_t seed){
    srand(seed);
}

uint8_t hal_random_generar(uint8_t min, uint8_t max) {
    if(max <= min) { 
        return min;
    }
    return (rand() % (max - min + 1)) + min;
}
