/* *****************************************************************************
 * P.H.2024: rt_fifo.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */

#include "rt_fifo.h"
#include "drv_monitor.h"
#include "drv_tiempo.h"
#include "drv_tiempo.h"


#define TAM_COLA 64


typedef struct {
	EVENTO_T ID_EVENTO;
	uint32_t auxData;
	Tiempo_us_t TS;
} EVENTO;


// Cola y variables de control
static EVENTO cola[TAM_COLA];
static uint8_t ultimo_tratado; 	   // Indice del último evento tratado
static uint8_t siguiente_a_tratar; // Índice del siguiente evento a tratar
static int8_t ultimo_insertado; 	   // ultimo dato insertado en la cola

// Monitor que marca el owerflow
static uint32_t MON_OVERFLOW;		

// Vector con las estadístas de cantidad de eventos
static uint32_t stats[EVENT_TYPES];

// Cantidad de eventos sin tratar
//static uint32_t eventos_sin_tratar;


void rt_FIFO_inicializar(uint32_t monitor_overflow){
    ultimo_tratado = 254;
    siguiente_a_tratar = 0;
    ultimo_insertado = -1;
		//eventos_sin_tratar = 0;

    for(uint8_t i = 0; i < EVENT_TYPES; i++){
        stats[i] = 0;
    }

    MON_OVERFLOW = monitor_overflow;

    drv_monitor_desmarcar(MON_OVERFLOW);
}

// Función que encola el evento dado al que le añade el timestamp, en caso de haber overflow lo marca mediante MON_OVERFLOW y se queda en bucle
void rt_FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData) {
    // Comprobar que sea un evento válido
    if (ID_evento < EVENT_TYPES && ID_evento > 0) {
        uint8_t siguiente_pos = (ultimo_insertado + 1) % TAM_COLA;

        // Detectar overflow: si siguiente posición es igual al índice siguiente a tratar
        if (ultimo_insertado != -1 && siguiente_pos == siguiente_a_tratar) {
            drv_monitor_marcar(MON_OVERFLOW);
            while (1); // Bucle infinito en caso de overflow
        }

        // Añadir el evento
        cola[siguiente_pos].ID_EVENTO = ID_evento;
        cola[siguiente_pos].auxData = auxData;
        cola[siguiente_pos].TS = (uint32_t)drv_tiempo_actual_us();

        // Actualizar el índice del último elemento insertado
        ultimo_insertado = (ultimo_insertado + 1) % TAM_COLA;

        // Actualizar estadísticas
        stats[ID_evento]++;
        stats[0]++;
    }
}

// Función que devuelve el número de eventos por procesar (incluyendo el propio evento extraido), y que pasa por los parámetros
// ID_evento, auxData, TS las variables relacionadas con el primer evento a tratar
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData, Tiempo_us_t *TS){
	uint8_t eventos_sin_tratar = 0;
    // Si hay eventos por tratar se devuelve el primero
    if(siguiente_a_tratar != (ultimo_insertado + 1) % TAM_COLA){
			if(ultimo_insertado >= siguiente_a_tratar){
						eventos_sin_tratar = ultimo_insertado - siguiente_a_tratar + 1;
				}
				else{
						eventos_sin_tratar = (TAM_COLA - siguiente_a_tratar + ultimo_insertado + 1);
				}
			
				// Variables del primer evento a tratar
        *ID_evento = cola[siguiente_a_tratar].ID_EVENTO;
        *auxData = cola[siguiente_a_tratar].auxData;
        *TS = cola[siguiente_a_tratar].TS;
        
        // Actualiza los índices de tratados y siguiente a tratar
        ultimo_tratado = siguiente_a_tratar;
        siguiente_a_tratar = (siguiente_a_tratar + 1) % TAM_COLA;
    } 
		return eventos_sin_tratar;
}

// Dado un evento, nos devuelve cuantas veces ha sido encolado (ev_VOID nos da el total)
// Si ID_evento esta fuera del rango de eventos posibles se devuelve 0
uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento){
    if(ID_evento < EVENT_TYPES) {
        return stats[ID_evento];
    } 
    else {
        return 0;
    }
}


// Función que comprueba que cuando hay overflow la cola se bloquea en un bucle y marca el monitor 4
void verificar_overflow_cola(){
		for(int i = 0; i < 65; i++){
				rt_FIFO_encolar((EVENTO_T)1, i);
		}
}
// Función que verifica la correcta inserción y extracción de eventos en la cola, así como sus estadísticas
int verificar_insertar_extraer_en_cola(){
		uint32_t eventos1 = 10;
		uint32_t eventos2 = 10;
	
		int resultado = 1;
	
		for(int i = 0; i < eventos1; i++){
				rt_FIFO_encolar(ev_T_PERIODICO, i);
		}
		
		for(int i = 0; i < eventos2; i++){
				rt_FIFO_encolar(ev_PULSAR_BOTON, i);
		}
		
		EVENTO_T id_evento;
		uint32_t auxData;
		Tiempo_us_t time;
		
		for(int i = 0; i < eventos1; i++) {
				rt_FIFO_extraer(&id_evento, &auxData, &time);
				if( id_evento != ev_T_PERIODICO || auxData != i ){
						resultado = 0;
				}
		}
		
		for(int i = 0; i < eventos2; i++) {
				rt_FIFO_extraer(&id_evento, &auxData, &time);
				if( id_evento != ev_PULSAR_BOTON || auxData != i ){
						resultado = 0;
				}
		}
		
		if((rt_FIFO_estadisticas((EVENTO_T)1) != eventos1 || rt_FIFO_estadisticas((EVENTO_T)2) != eventos2 || rt_FIFO_estadisticas((EVENTO_T)0) != (eventos1 + eventos2))) {
				resultado = 0;
		}
		
		return resultado;
}
