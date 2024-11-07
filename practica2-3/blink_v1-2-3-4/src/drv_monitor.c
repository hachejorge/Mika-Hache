#include "drv_monitor.h"
#include "hal_gpio.h"
#include "board.h"

#if MONITOR_NUMBER > 0
	static const uint8_t monitors_list[MONITOR_NUMBER] = MONITORS_LIST;
#endif

uint32_t drv_monitor_iniciar(){
    return MONITOR_NUMBER;
}

void drv_monitor_marcar(uint32_t id){
    #if MONITOR_NUMBER > 0
		if ((id <= MONITOR_NUMBER) && (id >0)) hal_gpio_escribir(monitors_list[id-1], MONITORS_ACTIVE_STATE);
    #endif //LEDS_NUMBER > 0		
}

void drv_monitor_desmarcar(uint32_t id){
    #if MONITOR_NUMBER > 0
		if ((id <= MONITOR_NUMBER) && (id >0)) hal_gpio_escribir(monitors_list[id-1], ~MONITORS_ACTIVE_STATE);
    #endif //LEDS_NUMBER > 0	
}

