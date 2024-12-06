#include "rt_GE.h"
#include "srv_alarm.h"
#include "rt_fifo.h"
#include "drv_consumo.h"
#include "drv_monitor.h"
#include "hal_WDT.h"
#include <stddef.h>
#include <stdbool.h>

EVENTO_T evs_USUARIO[ev_NUM_EV_USUARIO] = {ev_PULSAR_BOTON};

#ifdef DEBUG
// Tiempo durante el cual el procesador se encuentra en esperas en micro segundos
uint64_t tiempo_en_espera;
#endif

#define rt_GE_MAX_SUSCRITOS 4

typedef struct {
    void (*callbacks[rt_GE_MAX_SUSCRITOS])(EVENTO_T, uint32_t);
    uint8_t callbacks_usados;
} GE;

static GE gestor_eventos[EVENT_TYPES];

static uint32_t monitor_overflow;

bool esEventoUsuario(EVENTO_T id_evento){
    for(int i = 0; i < ev_NUM_EV_USUARIO; i++){
        if(evs_USUARIO[i] == id_evento) return true;
    }
    return false;
}

// si es un evento usuario reprogramamos el ev_INACTIVIDAD, si es un ev_INACTIVIDAD dormimos el procesador
void rt_GE_tratar(EVENTO_T evento, uint32_t auxiliar){
    if(esEventoUsuario(evento)){
        svc_alarma_activar(svc_alarma_codificar(0, 20*1000),ev_INACTIVIDAD,0); // programamos una alarma a los 20 segundos
    }
    else if(evento == ev_INACTIVIDAD){
        drv_consumo_dormir();	// dormimos el procesador
    }
}

// Inicializar sus estructuras (estáticas)
// suscribir EVs usuario (lista rt_evento_t)
// activa alarma ev_INACTIVIDAD
void rt_GE_iniciar(uint32_t monitor) {
    monitor_overflow = monitor;
    drv_consumo_iniciar(3,4);
    for(int i = 0; i < EVENT_TYPES; i++){
        gestor_eventos[i].callbacks_usados = 0;
        for(int j = 0; j < rt_GE_MAX_SUSCRITOS; j++){
            gestor_eventos[i].callbacks[j] = NULL;
        }
    }
		svc_GE_suscribir(ev_PULSAR_BOTON, rt_GE_tratar); // suscribe ev_PULSAR_BOTON a rt_GE_tratar
		svc_GE_suscribir(ev_INACTIVIDAD, rt_GE_tratar); // suscribe ev_INACTIVIDAD a rt_GE_tratar
}

void rt_GE_lanzador(){
    EVENTO_T evento;
    uint32_t auxiliar;
    Tiempo_us_t timestamp;
  
		hal_WDT_iniciar(1);
	
		svc_alarma_activar(svc_alarma_codificar(0, 20*1000),ev_INACTIVIDAD,0);
    
		// Bucle principal para procesar la cola de eventos
    while (1) {
        // Desencolar el siguiente evento
        if (rt_FIFO_extraer(&evento, &auxiliar, &timestamp)) { 
            // Alimenta el watchdog
						hal_WDT_feed();
            // Despachar el evento a las tareas suscritas
            for (int i = 0; i < gestor_eventos[evento].callbacks_usados; i++) {
                //if (gestor_eventos[evento].callbacks[i] != NULL) {
                    gestor_eventos[evento].callbacks[i](evento, auxiliar);
                //}
            }
        } else {
					#ifdef DEBUG
            uint64_t tiempo_ant = drv_tiempo_actual_us();
					#endif
            // No hay eventos, entrar en modo de espera
            drv_consumo_esperar();
					#ifdef DEBUG
            uint64_t tiempo_pos = drv_tiempo_actual_us();
            tiempo_en_espera += (tiempo_pos - tiempo_ant);
					#endif
        }
    }
}

void svc_GE_suscribir(EVENTO_T evento, void(*f_callback)(EVENTO_T id, uint32_t aux)){
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

void svc_GE_cancelar(EVENTO_T evento, void(*f_callback)(EVENTO_T id, uint32_t aux)){
    if(gestor_eventos[evento].callbacks_usados > 0) {
        for(int i = 0; i < rt_GE_MAX_SUSCRITOS; i++) { // recorremos el vector de eventos suscritos
            if(gestor_eventos[evento].callbacks[i] == f_callback) { // si esa componente del vector está vacía, suscribimos un nuevo evento
                gestor_eventos[evento].callbacks[i] = NULL;
                gestor_eventos[evento].callbacks_usados--;    // disminuimos el número de callbacks en ese evento
							
								for (int j = i; j < rt_GE_MAX_SUSCRITOS - 1; j++) {
                    gestor_eventos[evento].callbacks[j] = gestor_eventos[evento].callbacks[j + 1];
                }

                // Última posición ahora es NULL
                gestor_eventos[evento].callbacks[rt_GE_MAX_SUSCRITOS - 1] = NULL;
								
								break;
            }
        }
    }
}

