#include "nrf.h"
#include "drv_SC.h"
#include "hal_WDT.h"

void hal_WDT_iniciar(uint32_t sec) {
    // Configurar el tiempo de timeout del WDT. 
    NRF_WDT->CRV = 32768 * sec; //10 segundos a 32kHz, minimo -> 0x0Ful;
		NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);  

    // Habilitar el reinicio del microcontrolador en caso de timeout.
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;  //Enable reload register 0 ... 0x01ul;
    NRF_WDT->TASKS_START = 0x01ul;    // Iniciar el Watchdog Timer.

    hal_WDT_feed(); // Alimentar el WDT por primera vez. 
}

void hal_WDT_feed(void) {
		drv_SC_entrar_disable_irq();
    // Alimentar el WDT para evitar un reinicio.
    NRF_WDT->RR[0] = 0x6E524635; // Valor específico para recargar el canal 0.
		drv_SC_salir_enable_irq();
}

