#include "drv_consumo.h"
#include "hal_consumo.h"
#include "drv_monitor.h"

static volatile uint32_t NUM_MONITORES;	// Número totat de monitores
static volatile uint32_t MON_WAIT;		// ID del monitor asociado al modo de bajo consumo del procesador
static volatile uint32_t MON_DORMIR;	// ID del monitor asociado al modo "apagado" del procesador


void drv_consumo_iniciar(uint32_t mon_wait, uint32_t mon_dormir){
	MON_WAIT = mon_wait;
	MON_DORMIR = mon_dormir;
	NUM_MONITORES = drv_monitor_iniciar();
	hal_consumo_iniciar();
}

void drv_consumo_esperar(void){
	// Desmarcar monitor_wait si es posible
	if(NUM_MONITORES > 0 && MON_WAIT > 0 && MON_WAIT <= NUM_MONITORES) {
		drv_monitor_desmarcar(MON_WAIT);
	}
    hal_consumo_esperar();
	// Marcar monitor_wait si es posible
	if(NUM_MONITORES > 0 && MON_WAIT > 0 && MON_WAIT <= NUM_MONITORES) {
		drv_monitor_desmarcar(MON_WAIT);
	}
}

void drv_consumo_dormir(void){
    hal_consumo_dormir();
}
