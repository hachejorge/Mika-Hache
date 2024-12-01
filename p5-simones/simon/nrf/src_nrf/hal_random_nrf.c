/* *****************************************************************************
 * P.H.2024: hal_random_nrf.c
 * Nekane Diaz Montoya   870795
 * Jorge Hernandez Aznar 872838
 */

#include "hal_random.h"
#include "nrf.h"

void hal_random_iniciar(uint32_t seed){
    // Enable digital error correction
    NRF_RNG->CONFIG = RNG_CONFIG_DERCEN_Enabled << RNG_CONFIG_DERCEN_Pos;

    // Enable shortcut to stop the RNG after generating one value.
    NRF_RNG->SHORTS = RNG_SHORTS_VALRDY_STOP_Enabled << RNG_SHORTS_VALRDY_STOP_Pos;

    // Clear any existing VALRDY events and start generating new value
    NRF_RNG->EVENTS_VALRDY = 0;
    NRF_RNG->TASKS_START = 1;
}

uint8_t hal_random_generar(uint8_t min, uint8_t max){
    if(max <= min) {
        return min;
    }
    // Wait while the new random value is generated
    while (NRF_RNG->EVENTS_VALRDY == 0);
    NRF_RNG->EVENTS_VALRDY = 0;

    // Put generated random value into a variable.
    uint8_t random_value = NRF_RNG->VALUE;
    
    return (random_value % (max - min + 1)) + min; 
}
