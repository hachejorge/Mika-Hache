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
 * Si periodica es 0, la alarma es de 1 solo disparo; si es 1, es periodica.
 * Si retardo_ms es 0, desprograma la alarma; si ya estaba programada, se cambia el tiempo
 */
void svc_alarma_activar(uint8_t periodica, uint32_t retardo_ms, EVENTO_T ID_evento, uint32_t auxData);

/*
 * Desencola ev_T_periodicos y para ver si se ejecuta f_callback correctamente al vencer la 
 * alarma
 */
uint8_t ge_simple(int iter);

#endif
