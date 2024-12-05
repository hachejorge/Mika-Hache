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
				drv_SC_entrar_disable_irq();
        
				int periodica = (retardo_ms & 0x80000000) != 0;  // Verifica el bit más significativo.
				uint32_t retardo_real = retardo_ms & 0x7FFFFFFF; // Extrae el retardo ignorando el bit de mayor peso.
			
				uint8_t indice = 0;  // Índice de la alarma a gestionar
        uint8_t encontrada = 0;

        // Busca si ya existe una alarma asociada al evento
        while (!encontrada && indice < num_alarmas) { 
            if (lista_alarmas[indice].estado && lista_alarmas[indice].id_evento == ID_evento) {   
                encontrada = 1;
            } 
            else {
                indice++;
            }
        }
				drv_SC_salir_enable_irq();

        // Si la alarma se puede gestionar (crear o modificar)
        if (indice < MAX_ALARMAS) {
            if (retardo_ms != 0) {  // Configura o actualiza la alarma
								hal_WDT_feed();
								drv_SC_entrar_disable_irq();

                lista_alarmas[indice].estado = 1;
                lista_alarmas[indice].intervalo_ms = retardo_real;
                lista_alarmas[indice].es_periodica = periodica;
                lista_alarmas[indice].id_evento = ID_evento;
                lista_alarmas[indice].dato_aux = auxData;
                lista_alarmas[indice].contador = drv_tiempo_actual_ms() + retardo_real;

                if (!encontrada) {  // Si es una nueva alarma, aumenta el contador
                    num_alarmas++;
                }
								drv_SC_salir_enable_irq();
            }
            else if (encontrada) {  // Si `intervalo_ms` es 0, desactiva la alarma
								hal_WDT_feed();
								drv_SC_entrar_disable_irq();
                lista_alarmas[indice].estado = 0;
                // Compacta la lista eliminando la alarma desactivada
                while (indice < num_alarmas - 1) {
                    lista_alarmas[indice] = lista_alarmas[indice + 1];
                    indice++;
                }
                lista_alarmas[indice].estado = 0;
                num_alarmas--;
								drv_SC_salir_enable_irq();
            }
            else {
                drv_monitor_marcar(indicador_overflow); // Error: no se puede desactivar una alarma inexistente
                while (1) {}
            }
        } else { // Error: desbordamiento
            drv_monitor_marcar(indicador_overflow);
            while (1) {}
        }
    } 
    else {    
        drv_monitor_marcar(indicador_overflow); // Error: evento inválido
        while (1) {}
    }
}

/*
 * Revisa las alarmas activas, ejecuta la función callback para las vencidas y actualiza las periódicas.
 */
void svc_alarma_tratar(EVENTO_T id_evento, uint32_t dato_aux) {
    int j;
		drv_SC_entrar_disable_irq();
    for (int i = 0; i < num_alarmas; i++) {
        if (lista_alarmas[i].estado && dato_aux >= lista_alarmas[i].contador) {  // Alarma vencida
            callback_funcion(lista_alarmas[i].id_evento, lista_alarmas[i].dato_aux);   // Ejecuta el callback

            if (lista_alarmas[i].es_periodica) { // Reprograma si es periódica
                lista_alarmas[i].contador = dato_aux + lista_alarmas[i].intervalo_ms;
            }
            else {  // Si no es periódica, desactiva y compacta
                lista_alarmas[i].estado = 0;
                j = i;
                while (j < num_alarmas - 1) {
                    lista_alarmas[j] = lista_alarmas[j + 1];
                    j++;
                }
                lista_alarmas[j].estado = 0;
                num_alarmas--;
                i--;
            }
        }
    }
		drv_SC_salir_enable_irq();
}

uint32_t svc_alarma_codificar(int periodica, uint32_t tiempo) {
    // Si periodica es 1, el bit de mayor peso será 1; si es 0, será 0.
    uint32_t resultado = tiempo & 0x7FFFFFFF; // Borra el bit de mayor peso.
    if (periodica) {
        resultado |= 0x80000000; // Establece el bit de mayor peso en 1.
    }
    return resultado;
}

