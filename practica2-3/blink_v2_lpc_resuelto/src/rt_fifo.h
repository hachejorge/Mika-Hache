/* *****************************************************************************
 * P.H.2024: rt_fifo.h
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
  
#include <stdint.h>
#include "rt_evento_t.h"
#include "drv_tiempo.h"

#ifndef RT_FIFO
#define RT_FIFO

#define TAM_COLA 64

typedef struct {
	EVENTO_T ID_EVENTO;
	uint32_t auxData;
	Tiempo_us_t TS;
} EVENTO;

// Cola y variables de control
static EVENTO cola[TAM_COLA];
static uint8_t ultimo_tratado; 	   // Indice del último evento tratado
static uint8_t siguiente_a_tratar; // Índice del siguiente evento a tratar
static uint8_t indice_cola; 	   // ultimo dato insertado en la cola

// Monitor que marca el owerflow
static uint32_t MON_OVERFLOW;		

// Vector con las estadístas de cantidad de eventos
static uint32_t stats[EVENT_TYPES];

// Cantidad de eventos sin tratar
static uint32_t eventos_sin_tratar;

void rt_FIFO_inicializar(uint32_t monitor_overflow);

// Función que encola el evento dado al que le añade el timestamp, en caso de haber overflow lo marca mediante MON_OVERFLOW y se queda en bucle
void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData);

// Función que devuelve el número de eventos por procesar (incluyendo el propio evento extraido), y que pasa por los parámetros
// ID_evento, auxData, TS las variables relacionadas con el primer evento a tratar
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData, Tiempo_us_t *TS);

// Dado un evento, nos devuelve cuantas veces ha sido encolado (ev_VOID nos da el total)
uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento);

#endif

