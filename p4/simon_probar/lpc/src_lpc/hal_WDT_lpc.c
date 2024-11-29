#include "hal_WDT.h"
#include "LPC210x.h"
#include "drv_SC.h"

void hal_WDT_iniciar(uint32_t sec) {
	WDTC = sec * (15000000 / 4); // time‐out WatchDog.
	WDMOD = 0x03; // Habilito y configuro reinicio.
	hal_WDT_feed();
}
void hal_WDT_feed(void) { 
	drv_SC_entrar_disable_irq();
	WDFEED = 0xAA; // Alimento el WatchDog
	WDFEED = 0x55;
	drv_SC_salir_enable_irq();
}

