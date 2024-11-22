#include "srv_alarm.h"
#include "drv_monitor.h"
#include "drv_tiempo.h"
#include "rt_GE.h"

static ALARMA alarmas[svc_ALARMAS_MAX];

static uint32_t mon_overflow;
static uint8_t alarmas_activas;

void (*callback)(EVENTO_T, uint32_t); // Callback para encolar eventos en FIFO
static EVENTO_T id_evento_callback;
 
void cb_alarma(){
    callback(id_evento_callback, drv_tiempo_actual_ms());
}

void svc_alarma_iniciar(uint32_t overflow, void(*f_callback)(EVENTO_T id, uint32_t aux), EVENTO_T ID_evento){
    alarmas_activas = 0;
    mon_overflow = overflow;
    callback = f_callback; // Asignación de la función de callback
	id_evento_callback = ID_evento;
    
    svc_GE_suscribir(ev_T_PERIODICO, svc_alarma_tratar);

    for (int i = 0; i < svc_ALARMAS_MAX; i++) {
        alarmas[i].activa = false;
        alarmas[i].retardo_ms = 0;
        alarmas[i].periodica = false;
        alarmas[i].ID_evento = ev_VOID;
        alarmas[i].auxData = 0;
        alarmas[i].alarm_counter = 0;
    }

    // Cada cierto tiempo se invoca a la alarma
    drv_tiempo_periodico_ms(MIN_MS_INTERRUPT_ALARM, cb_alarma, ID_evento);

}

// ID_evento referencia al tipo de evento que se encolará al vencer la alarma
// retardo_ms en su bit de mayor peso indica si la alarma es periódica o no
// Si el retardo es cero se cancela la alarma
void svc_alarma_activar(uint32_t retardo_ms, EVENTO_T ID_evento, uint32_t auxData){
    bool periodica = (retardo_ms & 0x80000000) != 0;    // Bit más significativo indica periodicidad
    Tiempo_ms_t retardo_real = retardo_ms & 0x7FFFFFFF; // Retardo real sin el bit de periodica o no

    if(retardo_ms == 0){                              // si el retardo es igual a 0, se desactiva la alarma
        for(int i = 0; i < svc_ALARMAS_MAX; i++){     // Recorremos el vector de alarmas hasta encontrar la que tenga el mismo ID_evento 
            if(alarmas[i].ID_evento == ID_evento && alarmas[i].activa){ // además la alarma debe estar activa
                alarmas[i].activa = false;
                alarmas_activas--;
            }
        }
    }
    else{
        if(alarmas_activas < svc_ALARMAS_MAX){
					bool activacion_hecha = false;
					for(int i = 0; i < svc_ALARMAS_MAX && !activacion_hecha; i++){
                if(!alarmas[i].activa){
                    alarmas[i].activa = true;          
                    alarmas[i].retardo_ms = retardo_real;
                    alarmas[i].ID_evento = ID_evento;  
                    alarmas[i].auxData = auxData;      
                    alarmas[i].periodica = periodica;  
                    alarmas[i].alarm_counter = 0;
                    alarmas_activas++;   
					activacion_hecha = true;
                }
                // Añadir si la alarma ya está añadida para que se reinicie la cuenta
            }
        }
        else{
            drv_monitor_marcar(mon_overflow);          // marcamos el monitor 
            while(true);                               // bucle infinito
        }
    }    
}

void svc_alarma_tratar(EVENTO_T ID_evento, uint32_t auxData){
    for (int i = 0; i < svc_ALARMAS_MAX; i++) {
        if (alarmas[i].activa) {
            alarmas[i].alarm_counter++;
            // Verificar si el tipo de evento coincide y el retardo ha expirado
            //if (alarmas[i].ID_evento == ID_evento && 
						if (alarmas[i].retardo_ms <= alarmas[i].alarm_counter * MIN_MS_INTERRUPT_ALARM) {
                // Disparar el evento usando el callback
                callback(alarmas[i].ID_evento, alarmas[i].auxData);
                // Si es periódica, recalcular el retardo
                if (alarmas[i].periodica) {
                    alarmas[i].alarm_counter = 0 ;  // Reiniciar el contador para esta alarma
                } 
                else {
                    // Si no es periódica, desactivar la alarma
                    alarmas[i].activa = 0;
                    alarmas_activas--;
                }
            } 
        }
    }
}

uint32_t svc_alarma_codificar(bool periodico, uint32_t retardo){
		retardo &= 0x7FFFFFFF;
		if(periodico){
			  return (1U << 31) | retardo;
		}
		else{
				return (0U << 31) | retardo;
		}
}
