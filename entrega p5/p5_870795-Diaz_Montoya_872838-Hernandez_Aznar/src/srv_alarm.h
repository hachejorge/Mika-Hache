/* *****************************************************************************
 * P.H.2024: srv_alarm.h
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
#ifndef SVC_ALARM
#define SVC_ALARM

#include <stdint.h>
#include "rt_evento_t.h"

/**
 * Suscribe a ID_evento y enciende la activacion periodica
 */
void svc_alarma_iniciar(uint32_t monitor_overflow, void(*funcion_callback)(EVENTO_T ID_event, uint32_t auxData), EVENTO_T ID_evento);

/*
 * Programa la alarma con retardo_ms.
 * Si el bit de mayor peso de retardo_ms es 0, la alarma se ejecuta solo una vez,  si es 1, es periódica.
 * Si retardo_ms es 0, desprograma la alarma; si ya estaba programada, se cambia el tiempo
 */
void svc_alarma_activar(uint32_t retardo_ms, EVENTO_T ID_evento, uint32_t auxData);

/*
 * Devuelve la codificación de una alarma dependiendo si es periodica o no,
 * el resultado son los 31 bits de menor peso de tiempo y el bit de mayor peso
 * es el valor de periodica 1 o 0
 */
uint32_t svc_alarma_codificar(int periodica, uint32_t tiempo);

#endif
