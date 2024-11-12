#ifndef SRV_ALARM
#define SRV_ALARM

#include "drv_tiempo.h"
#include "rt_evento_t.h"
#include <stdint.h>
#include <stdbool.h>

#define svc_ALARMAS_MAX 4

// Cada cuanto llega la notificación periodica a la alarma
// Máximo común divisor de los periodos de las alarmas
#define MIN_MS_INTERRUPT_ALARM 5000

typedef struct {
	bool activa;
	bool periodica;
	Tiempo_ms_t retardo_ms;
	uint32_t alarm_counter;
  uint32_t auxData;
	EVENTO_T ID_evento;
} ALARMA;


void svc_alarma_iniciar(uint32_t overflow, void(*f_callback)(EVENTO_T id, uint32_t aux), uint32_t ID_evento);

// ID_evento referencia al tipo de evento que se encolará al vencer la alarma
// retardo_ms en su bit de mayor peso indica si la alarma es periódica o no
// Si el retardo es cero se cancela la alarma
void svc_alarma_activar(uint32_t retardo_ms, EVENTO_T ID_evento, uint32_t auxData);

 
void svc_alarma_tratar(EVENTO_T ID_evento, uint32_t auxData);

uint32_t svc_alarma_codificar(bool periodico, uint32_t retardo);

#endif
	
	
