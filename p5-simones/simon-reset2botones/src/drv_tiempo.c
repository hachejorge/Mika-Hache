/* *****************************************************************************
 * P.H.2024: Driver/Manejador de los temporizadores
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 
#include "drv_tiempo.h"
#include "hal_tiempo.h"

static volatile uint32_t HAL_TICKS2US_T0;	// factor de conversión para transformar el número de ticks a milisegundos		
static volatile uint32_t HAL_TICKS2US_T1 = 16;	// factor de conversión para transformar el número de ticks a milisegundos						
#define US2MS	1000						// constante de microsegundos a milisegundos

static void(*f_callback)(uint32_t id, uint32_t ms);		//puntero a funcion a llamar cuando salte la RSI (en modo irq)

static volatile uint32_t ID_EVENTO;	// ID del led que va a conmutarse en la función callback en un periodo de ms

void f_callback_driver(){
		f_callback(ID_EVENTO, drv_tiempo_actual_ms());
}

/**
 * inicializa el reloj y empieza a contar
 */
void drv_tiempo_iniciar(void){
	HAL_TICKS2US_T0 = hal_tiempo_iniciar_tick();
}

/**
 * tiempo desde que se inicio el temporizador en microsegundos
 */
Tiempo_us_t drv_tiempo_actual_us(void){
	// Obtener el tiempo actual en ticks y convertir a microsegundos
    uint64_t ticks = hal_tiempo_actual_tick();
    Tiempo_us_t tiempo_us = 0;
		if (HAL_TICKS2US_T0 != 0) {
				tiempo_us = (ticks / HAL_TICKS2US_T0);  // Convertir ticks a microsegundos
    }
		return tiempo_us;
}

/**
 * tiempo desde que se inicio el temporizador en milisegundos, devuelve el riempo en milisegundos
 */
Tiempo_ms_t drv_tiempo_actual_ms(void){
		Tiempo_ms_t tiempo_ms = 0;
		if(US2MS != 0){
			tiempo_ms = drv_tiempo_actual_us() / US2MS; // Convertir ticks a milisegundos
		}
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

/**
 * cada ms milisegundos lanza una interrupción en la que se invoca a la función funcion_callback
 */
void drv_tiempo_periodico_ms(Tiempo_ms_t ms, void(*funcion_encolar_evento)(uint32_t id, uint32_t ms), uint32_t ID_evento){
	ID_EVENTO = ID_evento;
	f_callback = funcion_encolar_evento;
	uint32_t ticks = ms * HAL_TICKS2US_T1 * US2MS;
	hal_tiempo_reloj_periodico_tick(ticks, f_callback_driver);
}

