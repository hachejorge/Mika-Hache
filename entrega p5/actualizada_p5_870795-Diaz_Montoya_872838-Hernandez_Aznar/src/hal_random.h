/* *****************************************************************************
 * P.H.2024: hal_random.h
 * Nekane Diaz Montoya   870795
 * Jorge Hernandez Aznar 872838
 */

#ifndef HAL_RANDOM
#define HAL_RANDOM

#include <stdint.h>

void hal_random_iniciar(uint32_t seed);

uint8_t hal_random_generar(uint8_t min, uint8_t max);

#endif
