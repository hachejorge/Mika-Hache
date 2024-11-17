/* *****************************************************************************
 * P.H.2024: rt_fifo.h
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
  
#include <stdint.h>
#include "drv_tiempo.h"
#include "rt_evento_t.h"

#ifndef RT_FIFO
#define RT_FIFO

void rt_FIFO_inicializar(uint32_t monitor_overflow);

// Función que encola el evento dado al que le añade el timestamp, en caso de haber overflow lo marca mediante MON_OVERFLOW y se queda en bucle
void rt_FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData);

// Función que devuelve el número de eventos por procesar (incluyendo el propio evento extraido), y que pasa por los parámetros
// ID_evento, auxData, TS las variables relacionadas con el primer evento a tratar
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData, Tiempo_us_t *TS);

// Dado un evento, nos devuelve cuantas veces ha sido encolado (ev_VOID nos da el total)
uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento);

// Función que comprueba que cuando hay overflow la cola se bloquea en un bucle y marca el monitor 4
void verificar_overflow_cola(uint32_t mon_overflow);

// Función que verifica la correcta inserción y extracción de eventos en la cola, así como sus estadísticas
int verificar_insertar_extraer_en_cola(uint32_t mon_overflow);

#endif

