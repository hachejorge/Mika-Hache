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
//static uint8_t ultimo_tratado; 	   // Indice del último evento tratado
static uint8_t siguiente_a_tratar; // Índice del siguiente evento a tratar
static int8_t indice_cola; 	   // ultimo dato insertado en la cola

// Monitor que marca el owerflow
static uint32_t MON_OVERFLOW;		

// Vector con las estadístas de cantidad de eventos
static uint32_t stats[EVENT_TYPES];

// Cantidad de eventos sin tratar
static uint32_t eventos_sin_tratar;


void rt_FIFO_inicializar(uint32_t monitor_overflow){
    //ultimo_tratado = 0;
    siguiente_a_tratar = 0;
    indice_cola = -1;
    eventos_sin_tratar = 0;

    for(uint8_t i = 0; i < EVENT_TYPES; i++){
        stats[i] = 0;
    }

    MON_OVERFLOW = monitor_overflow;

    drv_monitor_desmarcar(MON_OVERFLOW);
}

// Función que encola el evento dado al que le añade el timestamp, en caso de haber overflow lo marca mediante MON_OVERFLOW y se queda en bucle
void rt_FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData){
    // Se comprueba que sea un evento válido
    if(ID_evento < EVENT_TYPES && ID_evento > 0) {
        // Se aumenta el número de eventos sin tratar
        eventos_sin_tratar++;
        // Si los eventos sin tratar superan el tamaño de la cola hay overflow
        if( eventos_sin_tratar > TAM_COLA) { // eventos_sin_tratar (> OR >=) TAM_COLA
            drv_monitor_marcar(MON_OVERFLOW);
            while(1);
        }

        // Se actualiza la posición en la que se tiene que colocar
        uint8_t siguiente_pos = (indice_cola + 1) % TAM_COLA;
        
        // Se añade el evento
        cola[siguiente_pos].ID_EVENTO = (EVENTO_T)ID_evento;
        cola[siguiente_pos].auxData = auxData;
        cola[siguiente_pos].TS = (uint32_t)drv_tiempo_actual_us(); // Selecciona como timestap el tiempo actual en microsegundos
        
        // Se aumenta el índice del último elemento de la cola
        indice_cola = (indice_cola + 1) % TAM_COLA;
        
        stats[ID_evento]++; // Aumenta las veces encoladas del ID_evento
        stats[0]++;         // Aumenta los eventos totales encolados
    }
}

// Función que devuelve el número de eventos por procesar (incluyendo el propio evento extraido), y que pasa por los parámetros
// ID_evento, auxData, TS las variables relacionadas con el primer evento a tratar
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData, Tiempo_us_t *TS){
    // Se asignan los eventos sin tratar totales, incluyendo el posible a extraer 
    uint8_t eventos_sin_tratar_resultado = eventos_sin_tratar;

    // Si hay eventos por tratar se devuelve el primero
    if(eventos_sin_tratar > 0){
        // Variables del primer evento a tratar
        *ID_evento = cola[siguiente_a_tratar].ID_EVENTO;
        *auxData = cola[siguiente_a_tratar].auxData;
        *TS = cola[siguiente_a_tratar].TS;
        
        // Actualiza los índices de tratados y siguiente a tratar
        //ultimo_tratado = siguiente_a_tratar;
        siguiente_a_tratar = (siguiente_a_tratar + 1) % TAM_COLA;

        // Se reduce el número de eventos sin tratar
        eventos_sin_tratar--;
    }
    return eventos_sin_tratar_resultado; 
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
void verificar_overflow_cola(uint32_t mon_overflow){
		rt_FIFO_inicializar(mon_overflow);

		for(int i = 0; i < 65; i++){
				rt_FIFO_encolar((EVENTO_T)1, i);
		}
}
// Función que verifica la correcta inserción y extracción de eventos en la cola, así como sus estadísticas
int verificar_insertar_extraer_en_cola(uint32_t mon_overflow){
		rt_FIFO_inicializar(mon_overflow);
	
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
				if( id_evento != 1 || auxData != i ){
						resultado = 0;
				}
		}
		
		for(int i = 0; i < eventos2; i++) {
				rt_FIFO_extraer(&id_evento, &auxData, &time);
				if( id_evento != 2 || auxData != i ){
						resultado = 0;
				}
		}
		
		if((rt_FIFO_estadisticas((EVENTO_T)1) != eventos1 || rt_FIFO_estadisticas((EVENTO_T)2) != eventos2 || rt_FIFO_estadisticas((EVENTO_T)0) != (eventos1 + eventos2))) {
				resultado = 0;
		}
		
		return resultado;
}
