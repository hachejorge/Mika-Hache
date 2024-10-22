/* *****************************************************************************
 * P.H.2024: Driver/Manejador de los temporizadores
 * suministra los servicios independientemente del hardware
 * usa los servicos de hal_tiempo.h: 
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 
#include "drv_tiempo.h"
#include "hal_tiempo.h"

static volatile uint32_t HAL_TICKS2US = 0;	// factor de conversión para transformar el número de ticks a milisegundos						
#define US2MS	1000						// constante de microsegundos a milisegundos

/**
 * inicializa el reloj y empieza a contar
 */
void drv_tiempo_iniciar(void){
	HAL_TICKS2US = hal_tiempo_iniciar_tick();
}

/**
 * tiempo desde que se inicio el temporizador en microsegundos
 */
Tiempo_us_t drv_tiempo_actual_us(void){
	// Obtener el tiempo actual en ticks y convertir a microsegundos
    uint64_t ticks = hal_tiempo_actual_tick();
    Tiempo_us_t tiempo_us = (ticks / HAL_TICKS2US);  // Convertir ticks a microsegundos
    return tiempo_us;
}

/**
 * tiempo desde que se inicio el temporizador en milisegundos, devuelve el riempo en milisegundos
 */
Tiempo_ms_t drv_tiempo_actual_ms(void){
	uint64_t ticks = hal_tiempo_actual_tick();
	Tiempo_ms_t tiempo_ms = (ticks / (HAL_TICKS2US * US2MS)); // Convertir ticks a milisegundos
	return tiempo_ms;
}

/**
 * retardo: esperar un cierto tiempo en milisegundos
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms){
	Tiempo_ms_t tIni = hal_tiempo_actual_tick(); // Se obtiene el tiempo actual tIni
	while((drv_tiempo_actual_ms() - tIni) < ms); // Espera hasta que el tiempo Actual menos el inicial sea menor que el dado
}

/**
 * esperar hasta un determinado tiempo (en ms), devuelve el tiempo actual
 */
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms){
	while(drv_tiempo_actual_ms() < ms);	// Mientras que el tiempo actual sea menor que el dado espera
	return drv_tiempo_actual_ms();
}
