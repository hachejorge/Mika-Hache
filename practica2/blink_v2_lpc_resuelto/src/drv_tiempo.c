/* *****************************************************************************
 * P.H.2024: Driver/Manejador de los temporizadores
 * suministra los servicios independientemente del hardware
 *
 * usa los servicos de hal_tiempo.h: 
 */
 
#include "drv_tiempo.h"
#include "hal_tiempo.h"

#define HAL_TICKS2US	  15 							// funcionamos PCLK a 15 MHz de un total de 60 MHz CPU Clock


#define TODO 0	//pa que no de error de compilacion con el proyecto vacio, modicicar
/**
 * nicializa el reloj y empieza a contar
 */
void drv_tiempo_iniciar(void){
	hal_tiempo_iniciar_tick();
}

/**
 * tiempo desde que se inicio el temporizador en microsegundos
 */
Tiempo_us_t drv_tiempo_actual_us(void){
	// Obtener el tiempo actual en ticks y convertir a microsegundos
    uint64_t ticks = hal_tiempo_actual_tick();
    Tiempo_us_t tiempo_us = (Tiempo_us_t)(ticks * HAL_TICKS2US);  // Convertir ticks a microsegundos
    return tiempo_us;
}

/**
 * tiempo desde que se inicio el temporizador en milisegundos
 */
Tiempo_ms_t drv_tiempo_actual_ms(void){
	return TODO;
}

/**
 * retardo: esperar un cierto tiempo en milisegundos
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms){
	
}

/**
 * esperar hasta un determinado tiempo (en ms), devuelve el tiempo actual
 */
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms){
	return TODO;
}
