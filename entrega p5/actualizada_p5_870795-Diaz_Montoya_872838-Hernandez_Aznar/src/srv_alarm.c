/* *****************************************************************************
 * P.H.2024: srv_alarm.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */

#include "srv_alarm.h"
#include "drv_tiempo.h"
#include "drv_monitor.h"
#include "drv_SC.h"
#include "rt_evento_t.h"
#include "rt_GE.h"
#include "hal_WDT.h"
#include <stdbool.h>

#define MAX_ALARMAS 5  // Máximo número de alarmas activas permitido
#define INTERVALO_TIMER_MS 1 // Periodo en milisegundos para las interrupciones del temporizador

/*
 *  Estructura para gestionar una alarma
 */
typedef struct {
    uint8_t estado;        // Indicador de si la alarma está activa
    uint32_t intervalo_ms; // Intervalo de tiempo entre disparos (para alarmas periódicas)
    uint8_t es_periodica;  // Indicador de periodicidad (1 si es periódica, 0 si es única)
    EVENTO_T id_evento;    // Identificador del evento asociado a la alarma
    uint32_t dato_aux;     // Información adicional asociada al evento
    uint32_t contador;     // Tiempo en el que debe dispararse la alarma
} ALARMA;

static ALARMA lista_alarmas[MAX_ALARMAS]; // Lista de alarmas disponibles
static uint32_t indicador_overflow;      // Indica si el límite de alarmas ha sido excedido
static void(*callback_funcion)(EVENTO_T id_evento, uint32_t dato_aux); // Puntero a la función que se llama al disparar una alarma
static uint32_t num_alarmas;             // Número actual de alarmas activas

void svc_alarma_tratar(EVENTO_T id_evento, uint32_t dato_aux);

/**
 * Inicializa el sistema de alarmas, suscribiendo un evento y configurando la función callback.
 */
void svc_alarma_iniciar(uint32_t monitor_overflow, void(*funcion_callback)(EVENTO_T id_evento, uint32_t dato_aux), EVENTO_T id_evento) {
    svc_GE_suscribir(id_evento, svc_alarma_tratar);    // Suscripción al evento
    // Inicialización de las alarmas y variables
    callback_funcion = funcion_callback;

    for (int i = 0; i < MAX_ALARMAS; i++) {
        lista_alarmas[i].estado = 0;  // Todas las alarmas comienzan inactivas
    }
    num_alarmas = 0;

    indicador_overflow = monitor_overflow;    // Marca para indicar desbordamiento
    drv_monitor_desmarcar(monitor_overflow);
    
    drv_tiempo_periodico_ms(INTERVALO_TIMER_MS, funcion_callback, id_evento); // Configura el temporizador
}


void svc_alarma_activar(uint32_t retardo_ms, EVENTO_T ID_evento, uint32_t auxData){
    if (ID_evento < EVENT_TYPES) {    // Verifica que el evento sea válido
			
				int periodica = (retardo_ms & 0x80000000) != 0;  // Verifica el bit más significativo.
				uint32_t retardo_real = retardo_ms & 0x7FFFFFFF; // Extrae el retardo ignorando el bit de mayor peso.
				
				bool encontrada = false;
				
				// Se busca entre las alarmas activas
				for(int i = 0; i < num_alarmas; i++){
						if(lista_alarmas[i].id_evento == ID_evento){
								encontrada = true;
								if(retardo_real == 0){
										lista_alarmas[i].estado = 0; // Se desactiva la alarma
								}
								else{
										lista_alarmas[i].estado = 1; // Se reactiva la alarma por si acaso
										lista_alarmas[i].intervalo_ms = retardo_real;  // Se actualiza el retardo
										lista_alarmas[i].es_periodica = periodica;			// Si es periodica o no
										lista_alarmas[i].dato_aux = auxData;						// Campo auxiliar de la alarma
										lista_alarmas[i].contador = drv_tiempo_actual_ms() + retardo_real;
								}
						}
				}
				// Si no está entre las alarmas previas
				if(!encontrada){
						// Hay hueco para la alarma
						if(num_alarmas < MAX_ALARMAS){
								lista_alarmas[num_alarmas].estado = 1;
								lista_alarmas[num_alarmas].intervalo_ms = retardo_real;
                lista_alarmas[num_alarmas].es_periodica = periodica;
                lista_alarmas[num_alarmas].id_evento = ID_evento;
                lista_alarmas[num_alarmas].dato_aux = auxData;
                lista_alarmas[num_alarmas].contador = drv_tiempo_actual_ms() + retardo_real;
							
								num_alarmas++;
						}
						// Se produce overflow en las alarmas
						else{
								drv_monitor_marcar(indicador_overflow);
								while(true);
						}
				}
		}
}

/*
 * Revisa las alarmas activas, ejecuta la función callback para las vencidas y actualiza las periódicas.
 */
void svc_alarma_tratar(EVENTO_T id_evento, uint32_t dato_aux) {
    for (int i = 0; i < num_alarmas; i++) {
        if (lista_alarmas[i].estado && dato_aux >= lista_alarmas[i].contador) {  // Alarma vencida
            callback_funcion(lista_alarmas[i].id_evento, lista_alarmas[i].dato_aux);   // Ejecuta el callback

            if (lista_alarmas[i].es_periodica) { // Reprograma si es periódica
                lista_alarmas[i].contador = dato_aux + lista_alarmas[i].intervalo_ms;
            }
            else {  // Si no es periódica, desactiva y compacta
                lista_alarmas[i].estado = 0;
                
								// Compactar usando un bucle for
                for (int j = i; j < num_alarmas - 1; j++) {
                    lista_alarmas[j] = lista_alarmas[j + 1];
                }
                num_alarmas--;
								
                i--; // Ajusta el índice para verificar la posición actual tras compactar
            }
        }
    }
}

uint32_t svc_alarma_codificar(int periodica, uint32_t tiempo) {
    // Si periodica es 1, el bit de mayor peso será 1; si es 0, será 0.
    uint32_t resultado = tiempo & 0x7FFFFFFF; // Borra el bit de mayor peso.
    if (periodica) {
        resultado |= 0x80000000; // Establece el bit de mayor peso en 1.
    }
    return resultado;
}

