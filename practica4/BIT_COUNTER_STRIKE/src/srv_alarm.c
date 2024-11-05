#include "srv_alarm.h"
#include "drv_monitor.h"

static ALARMA alarmas[svc_ALARMAS_MAX];

static uint32_t mon_overflow;
static uint8_t alarmas_activas;
void (*callback)(uint32_t, uint32_t); // Callback para encolar eventos en FIFO


void svc_alarma_iniciar(uint32_t overflow, void(*f_callback)(uint32_t id, uint32_t aux), uint32_t ID_evento){
    alarmas_activas = 0;
    mon_overflow = overflow;
    callback = f_callback; // Asignación de la función de callback

    for (int i = 0; i < svc_ALARMAS_MAX; i++) {
        alarmas[i].activa = false;
        alarmas[i].retardo_ms = 0;
        alarmas[i].periodica = false;
        alarmas[i].ID_evento = 0;
        alarmas[i].auxData = 0;
    }

}

// ID_evento referencia al tipo de evento que se encolará al vencer la alarma
// retardo_ms en su bit de mayor peso indica si la alarma es periódica o no
// Si el retardo es cero se cancela la alarma
void svc_alarma_activar(uint32_t retardo_ms, EVENTO_T ID_evento, uint32_t auxData){
    bool periodica = (retardo_ms & 0x80000000) != 0;  // Bit más significativo indica periodicidad

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
            for(int i = 0; i < svc_ALARMAS_MAX; i++){
                if(!alarmas[i].activa){
                    alarmas[i].activa = true;          // 
                    alarmas[i].retardo_ms = retardo_ms;//                   
                    alarmas[i].ID_evento = ID_evento;  //
                    alarmas[i].auxData = auxData;      //
                    alarmas[i].periodica = periodica;  //
                    alarmas_activas++;                 //
                }
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
        if (alarmas[i].activa && alarmas[i].ID_evento == ID_evento) {
            // Verificar si el retardo ha expirado
            if (alarmas[i].retardo_ms <= auxData) {
                // Disparar el evento usando el callback
                callback(alarmas[i].ID_evento, alarmas[i].auxData);
                // Si es periódica, recalcular el retardo
                if (alarmas[i].periodica) {
                    alarmas[i].retardo_ms += auxData;  // Recalcular el retardo para la próxima activación
                } 
                else {
                    // Si no es periódica, desactivar la alarma
                    alarmas[i].activa = 0;
                    alarmas_activas--;
                }
            } else {
                // Reducir el retardo en función del tiempo transcurrido (auxData)
                alarmas[i].retardo_ms -= auxData;
            }
        }
    }
}
