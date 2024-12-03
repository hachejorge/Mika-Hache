#include "rt_GE.h"
#include "srv_alarm.h"
#include "rt_fifo.h"
#include "drv_consumo.h"
#include "drv_monitor.h"
#include "hal_WDT.h"
#include <stddef.h>
#include <stdbool.h>

EVENTO_T evs_USUARIO[ev_NUM_EV_USUARIO] = {ev_PULSAR_BOTON};

static GE gestor_eventos[EVENT_TYPES];
static uint32_t monitor_overflow;

bool esEventoUsuario(EVENTO_T id_evento){
    for(int i = 0; i < ev_NUM_EV_USUARIO; i++){
        if(evs_USUARIO[i] == id_evento) return true;
    }
    return false;
}


void rt_GE_iniciar(uint32_t monitor) {
    monitor_overflow = monitor;
    for(int i = 0; i < EVENT_TYPES; i++){
        gestor_eventos[i].callbacks_usados = 0;
        for(int j = 0; j < rt_GE_MAX_SUSCRITOS; j++){
            gestor_eventos[i].callbacks[j] = NULL;
        }
    }
		svc_GE_suscribir(ev_PULSAR_BOTON, rt_GE_tratar);
		svc_GE_suscribir(ev_INACTIVIDAD, rt_GE_tratar);
}

void rt_GE_lanzador(){
    EVENTO_T evento;
    uint32_t auxiliar;
    Tiempo_us_t timestamp;
    
		hal_WDT_iniciar(10);
	
		svc_alarma_activar(svc_alarma_codificar(0, 20*1000),ev_INACTIVIDAD,0);
    
		// Bucle principal para procesar la cola de eventos
    while (1) {
        // Desencolar el siguiente evento
        if (rt_FIFO_extraer(&evento, &auxiliar, &timestamp)) { 
            
            // Despachar el evento a las tareas suscritas
            for (int i = 0; i < rt_GE_MAX_SUSCRITOS; i++) {
                if (gestor_eventos[evento].callbacks[i] != NULL) {
                    gestor_eventos[evento].callbacks[i](evento, auxiliar);
                }
            }
        } else {
            // No hay eventos, entrar en modo de espera
            drv_consumo_esperar();
				}
    }
}

void svc_GE_suscribir(EVENTO_T evento, void(*f_callback)(EVENTO_T id, uint32_t aux)){
    if (gestor_eventos[evento].callbacks_usados >= rt_GE_MAX_SUSCRITOS){
        // marcamos el monitor ya que hay overflow
        drv_monitor_marcar(monitor_overflow);
        while(true); 
    }else{
				hal_WDT_feed();
			
        bool evento_suscrito = false;
        for(int i = 0; i < rt_GE_MAX_SUSCRITOS && !evento_suscrito; i++) { // recorremos el vector de eventos suscritos
            if(gestor_eventos[evento].callbacks[i] == NULL) { // si esa componente del vector está vacía, suscribimos un nuevo evento
                gestor_eventos[evento].callbacks[i] = f_callback; // habría que jacer aquí svc_GE_candelar?????
                evento_suscrito = true;                       // el evento está suscrito
                gestor_eventos[evento].callbacks_usados++;    // aumentamos el número de callbacks en ese evento
            }
        }
    }
}

void svc_GE_cancelar(EVENTO_T evento, void(*f_callback)(EVENTO_T id, uint32_t aux)){
    if(gestor_eventos[evento].callbacks_usados > 0) {
        for(int i = 0; i < rt_GE_MAX_SUSCRITOS; i++) { // recorremos el vector de eventos suscritos
            if(gestor_eventos[evento].callbacks[i] == f_callback) { // si esa componente del vector está vacía, suscribimos un nuevo evento
                gestor_eventos[evento].callbacks[i] = NULL;
                gestor_eventos[evento].callbacks_usados--;    // disminuimos el número de callbacks en ese evento
            }
        }
    }
}



void rt_GE_tratar(EVENTO_T evento, uint32_t auxiliar){
    if(esEventoUsuario(evento)){
        svc_alarma_activar(svc_alarma_codificar(0, 20*1000),ev_INACTIVIDAD,0);
    }
    else if(evento == ev_INACTIVIDAD){
        drv_consumo_dormir();
    }
}
