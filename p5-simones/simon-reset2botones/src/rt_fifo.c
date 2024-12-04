/* *****************************************************************************
 * P.H.2024: rt_fifo.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */

#include "rt_fifo.h"
#include "drv_monitor.h"
#include "drv_SC.h"
#include "drv_tiempo.h"
#include "hal_WDT.h"
#include "stdio.h"

#define TAM_COLA 64

// Cola y variables de control
static EVENTO cola[TAM_COLA];
static uint8_t siguiente_a_tratar; // Índice del siguiente evento a tratar
static int8_t indice_cola; 	   // ultimo dato insertado en la cola

// Monitor que marca el owerflow
static uint32_t MON_OVERFLOW;		

// Vector con las estadístas de cantidad de eventos
static uint32_t stats[EVENT_TYPES];

// Cantidad de eventos sin tratar
static uint32_t eventos_sin_tratar;
#ifdef DEBUG
/*** ESTADÍSTICAS ***/
// Cantidad de eventos sin tratar de forma simultánea en la cola
static uint32_t max_eventos_sin_tratar;

// Total de eventos tratados
static uint32_t total_eventos_tratados;

// Tiempo medio de espera hasta tratar un evento
static uint32_t avg_tiempo_espera;

// Máximo de tiempo esperado hasta tratar un evento
static uint32_t max_tiempo_espera;

// Tiempo de respuesta del usuario desde que se marca la secuencia hasta que se empieza a pulsar
static uint32_t max_respuesta_usuario;

#endif

void rt_FIFO_inicializar(uint32_t monitor_overflow){
    siguiente_a_tratar = 0;
    indice_cola = -1;
    eventos_sin_tratar = 0;
	#ifdef DEBUG
		max_eventos_sin_tratar = 0;
		total_eventos_tratados = 0;
		avg_tiempo_espera = 0;
		max_tiempo_espera = 0;
		max_respuesta_usuario = 0;
	#endif
    for(uint8_t i = 0; i < EVENT_TYPES; i++){
        stats[i] = 0;
    }

    MON_OVERFLOW = monitor_overflow;

    drv_monitor_desmarcar(MON_OVERFLOW);
}

// Función que encola el evento dado al que le añade el timestamp, en caso de haber overflow lo marca mediante MON_OVERFLOW y se queda en bucle
void rt_FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData){
		drv_SC_entrar_disable_irq();
    // Se comprueba que sea un evento válido
    if(ID_evento < EVENT_TYPES && ID_evento > 0) {
        // Se aumenta el número de eventos sin tratar
        eventos_sin_tratar++;
        // Si los eventos sin tratar superan el tamaño de la cola hay overflow
        if( eventos_sin_tratar > TAM_COLA) { // eventos_sin_tratar (> OR >=) TAM_COLA
            drv_monitor_marcar(MON_OVERFLOW);
            while(1);
        }
				hal_WDT_feed();
				
        // Se actualiza la posición en la que se tiene que colocar
        uint8_t siguiente_pos = (indice_cola + 1) % TAM_COLA;
        
        // Se añade el evento
        cola[siguiente_pos].ID_EVENTO = (EVENTO_T)ID_evento;
        cola[siguiente_pos].auxData = auxData;
        cola[siguiente_pos].TS = (uint32_t)drv_tiempo_actual_us(); // Selecciona como timestap el tiempo actual en microsegundos
        
        // Se aumenta el índice del último elemento de la cola
        indice_cola = (indice_cola + 1) % TAM_COLA;
        #ifdef DEBUG
				if(max_eventos_sin_tratar < eventos_sin_tratar){
						max_eventos_sin_tratar = eventos_sin_tratar;
				}
				#endif
        stats[ID_evento]++; // Aumenta las veces encoladas del ID_evento
        stats[0]++;         // Aumenta los eventos totales encolados
    }
		drv_SC_salir_enable_irq();
}

// Función que devuelve el número de eventos por procesar (incluyendo el propio evento extraido), y que pasa por los parámetros
// ID_evento, auxData, TS las variables relacionadas con el primer evento a tratar
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData, Tiempo_us_t *TS){
		drv_SC_entrar_disable_irq();
		
		// Se asignan los eventos sin tratar totales, incluyendo el posible a extraer 
    uint8_t eventos_sin_tratar_resultado = eventos_sin_tratar;
	

    // Si hay eventos por tratar se devuelve el primero
    if(eventos_sin_tratar > 0){
        // Variables del primer evento a tratar
        *ID_evento = cola[siguiente_a_tratar].ID_EVENTO;
        *auxData = cola[siguiente_a_tratar].auxData;
        *TS = cola[siguiente_a_tratar].TS;
        
        // Actualiza siguiente a tratar
        siguiente_a_tratar = (siguiente_a_tratar + 1) % TAM_COLA;

        // Se reduce el número de eventos sin tratar
        eventos_sin_tratar--;
			#ifdef DEBUG
				/**** ACTUALIZACIÓN ESTÁDISTICAS ****/
				uint64_t tiempo_total_espera = avg_tiempo_espera * total_eventos_tratados;
				
				// Hacemos cast a 32 bits  ya que se supone que el tiempo de espera es muy poco en comparación al TS, o al tiempo actual
				uint32_t tiempo_espera_actual = drv_tiempo_actual_us() - *TS;
				
				total_eventos_tratados++;
				
				// Actualizamos el tiempo de espera hasta procesar un evento
				avg_tiempo_espera = (tiempo_total_espera + tiempo_espera_actual)/total_eventos_tratados;
    
				if(max_tiempo_espera < tiempo_espera_actual){
						max_tiempo_espera = tiempo_espera_actual;
				}
				// Inicia secuencia guarda el momento en el que proceso el ev_JUEGO_SEQ_JUEGO que es el que provocará que se muestre la secuencia de juego
				uint32_t inicia_secuencia = 0;
				if(*ID_evento == ev_JUEGO_SEQ_JUEGO){
						inicia_secuencia = drv_tiempo_actual_us();
				}
				
				uint32_t tiempo_respuesta_usuario;
				// El tiempo_respuesta_usuario será la diferencia entre el tiempo en el momento en el que se produzca una reacción del usuario (ev_PULSAR_BOTON y el inicio de la secuencia)
				if(*ID_evento == ev_PULSAR_BOTON){
						 tiempo_respuesta_usuario = drv_tiempo_actual_us() - inicia_secuencia;
					if(max_respuesta_usuario < tiempo_respuesta_usuario){
						max_respuesta_usuario = tiempo_respuesta_usuario;
					}
				}
				
				#endif
		}
		drv_SC_salir_enable_irq();
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
void verificar_overflow_cola(){
		for(int i = 0; i < 65; i++){
				rt_FIFO_encolar(ev_T_PERIODICO, i);
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
