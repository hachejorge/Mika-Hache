#include "rt_GE.h"
#include "srv_alarm.h"
#include "rt_fifo.h"
#include "drv_consumo.h"
#include <stddef.h>

static GE gestor_eventos[EVENT_TYPES];
static uint32_t monitor_overflow;

void rt_GE_iniciar(uint32_t monitor) {
    monitor_overflow = monitor;
    for(int i = 0; i < EVENT_TYPES; i++){
        gestor_eventos[i].callbacks_usados = 0;
        for(int j = 0; j < rt_GE_MAX_SUSCRITOS; j++){
            gestor_eventos[i].callbacks[j] = NULL;
        }
    }
    svc_alarma_activar(20000,ev_INACTIVIDAD,0);
}

void rt_GE_lanzador(){
    EVENTO_T evento;
    uint32_t auxiliar;
    Tiempo_us_t timestamp;
 
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
            drv_consumo_dormir();
        }
    }
}

void svc_GE_suscribir(EVENTO_T evento, void(*f_callback)(uint32_t id, uint32_t aux)){
    if (gestor_eventos[evento].callbacks_usados >= rt_GE_MAX_SUSCRITOS){
        // marcamos el monitor ya que hay overflow
        drv_monitor_marcar(monitor_overflow);
        while(true); 
    }else{
        bool evento_suscrito = false;
        for(int i = 0; i < rt_GE_MAX_SUSCRITOS && !evento_suscrito; i++) { // recorremos el vector de eventos suscritos
            if(gestor_eventos[evento].callbacks[i] == NULL) { // si esa componente del vector está vacía, suscribimos un nuevo evento
                gestor_eventos[evento].callbacks[i] = f_callback;
                evento_suscrito = true;                       // el evento está suscrito
                gestor_eventos[evento].callbacks_usados++;    // aumentamos el número de callbacks en ese evento
            }
        }
    }
}

void svc_GE_cancelar(EVENTO_T evento, void(*f_callback)(uint32_t id, uint32_t aux)){
    if(gestor_eventos[evento].callbacks_usados > 0) {
        for(int i = 0; i < rt_GE_MAX_SUSCRITOS; i++) { // recorremos el vector de eventos suscritos
            if(gestor_eventos[evento].callbacks[i] == f_callback) { // si esa componente del vector está vacía, suscribimos un nuevo evento
                gestor_eventos[evento].callbacks[i] = NULL;
                gestor_eventos[evento].callbacks_usados--;    // aumentamos el número de callbacks en ese evento
            }
        }
    }
}

bool esEventoUsuario(EVENTO_T id_evento){
    for(int i = 0; i < ev_NUM_EV_USUARIO; i++){
        if(evs_USUARIO[i] == id_evento) return true;
    }
    return false;
}

void rt_GE_tratar(EVENTO_T evento, uint32_t auxiliar){
    if(esEventoUsuario(evento)){
        svc_alarma_activar(20000, ev_INACTIVIDAD, 0);
    }
    else if(evento == ev_INACTIVIDAD){
        drv_consumo_dormir();
    }
}
