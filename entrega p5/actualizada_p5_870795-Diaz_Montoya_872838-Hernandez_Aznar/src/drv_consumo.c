/* *****************************************************************************
 * P.H.2024: drv_cosumo.c
 * implementación del módulo drv_consumo
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 
#include "drv_consumo.h"
#include "hal_consumo.h"
#include "drv_monitor.h"
#include "drv_leds.h"

static uint32_t NUM_MONITORES;	// Número total de monitores
static uint32_t MON_WAIT;		// ID del monitor asociado al modo de bajo consumo del procesador
static uint32_t MON_DORMIR;	// ID del monitor asociado al modo apagado del procesador


void drv_consumo_iniciar(uint32_t mon_wait, uint32_t mon_dormir){
	// Asignamos a los monitores sus valores e iniciamos los momnitores
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
		drv_monitor_marcar(MON_WAIT);
	}
}

void drv_consumo_dormir(void){
	// Desmarcar monitor_dormir si es posible
	if(NUM_MONITORES > 0 && MON_DORMIR > 0 && MON_DORMIR <= NUM_MONITORES) {
		drv_monitor_desmarcar(MON_DORMIR);
	}
	// Apagamos todos los leds antes de ir a dormir
	drv_leds_apagar_todos();
	
  hal_consumo_dormir();
	
	// Marcar monitor_dormir si es posible
	if(NUM_MONITORES > 0 && MON_DORMIR > 0 && MON_DORMIR <= NUM_MONITORES) {
		drv_monitor_marcar(MON_DORMIR);
	}
}


