/* *****************************************************************************
 * P.H.2024: rt_evento_t.h
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */

#ifndef RT_EVENTO_T
#define RT_EVENTO_T

typedef enum {
	ev_VOID = 0, 	    	// default, vacio
	ev_T_PERIODICO = 1,  	// noficación del timer cada cierto tiempo
	ev_PULSAR_BOTON = 2,	// pulsación de un botón
	ev_INACTIVIDAD = 3,  	// evento de parar el procesador
	ev_BOTON_RETARDO = 4,	// evento para el retardo del autómata de un botón
	ev_INICIAR_JUEGO = 5,
	ev_LEDS_ENCENDER = 6,
	ev_LEDS_APAGAR = 7,
	ev_JUEGO_CONTINUAR = 8,
	ev_JUEGO_SEQ_INCIAL = 9,
	ev_JUEGO_SEQ_JUEGO = 10,
	ev_LEDS_SIGUINTE = 11,
	ev_TIEMPO_SUPERADO = 12,
	ev_APAGAR_LEDS = 13,
} EVENTO_T;			    //mapea a uint32_t

#define EVENT_TYPES 14

#define ev_NUM_EV_USUARIO 1
#define ev_USUARIO {ev_PULSAR_BOTON}


#endif

