#include "hal_WDT.h"
#include "LPC210x.h"

void hal_WDT_iniciar(uint32_t sec) {
	WDTC = sec * (15000000 / 4); // time‐out WatchDog.
	WDMOD = 0x03; // Habilito y configuro reinicio.
	hal_WDT_feed();
}
void hal_WDT_feed(void) { //ojo irq();
	WDFEED = 0xAA; // Alimento el WatchDog
	WDFEED = 0x55;
}

