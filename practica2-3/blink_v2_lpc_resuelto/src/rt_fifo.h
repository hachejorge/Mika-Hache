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

#define TAM_COLA 10

typedef struct {
	EVENTO_T ID_EVENTO;
	uint32_t auxData;
	Tiempo_us_t TS;
} EVENTO;



// Cola y variables de control
static EVENTO cola[TAM_COLA];
static uint8_t ultimo_tratado; //
static uint8_t siguiente_a_tratar; //
static uint8_t indice_cola; // ultimo dato insertado en la cola

// Monitor que marca el owerflow
static uint32_t MON_OVERFLOW;		

// Vector con las estadístas de cantidad de eventos
static uint32_t stats[EVENT_TYPES];

static uint32_t eventos_sin_tratar;

void rt_FIFO_inicializar(uint32_t monitor_overflow);

void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData);

uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData, Tiempo_us_t *TS);

uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento);

#endif
