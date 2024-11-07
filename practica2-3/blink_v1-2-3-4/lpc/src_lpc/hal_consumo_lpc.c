#include "hal_consumo.h"
#include <LPC210x.H>                       /* LPC210x definitions */

void hal_consumo_iniciar(){

}

// Codigo obtenido de los ejemplos en moodle
// void PM_power_wait (void)  {
// 	EXTWAKE = 1; // EXTINT0 will awake the processor
// 	PCON |= 0x01; 
// }

// // Set the processor into power down state 
// // The watchdog cannot wake up the processor from power down
// void PM_power_down (void)  {
// 	EXTWAKE = 1; // EXTINT0 will awake the processor
// 	PCON |= 0x02; 
// 	switch_to_PLL(); //PLL aranca a 12Mhz cuando volvemos de powewr down
// }


void hal_consumo_esperar(){
    EXTWAKE = 1; // EXTINT0 will awake the processor
	  PCON |= 0x01; 
}

void hal_consumo_dormir(){}
	
