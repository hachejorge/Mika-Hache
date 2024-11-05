/* *****************************************************************************
 * P.H.2024: rt_evento_t.h
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */

#ifndef RT_EVENTO_T
#define RT_EVENTO_T

typedef enum {
	ev_VOID = 0, 	    	 // default, vacio
	ev_T_PERIODICO = 1,  // noficación del timer cada cierto tiempo
	ev_PULSAR_BOTON = 2, // pulsación de un botón
	ev_INACTIVIDAD = 3,  // evento de parar el procesador
} EVENTO_T;			    //mapea a uint32_t

#define EVENT_TYPES 4

#define ev_NUM_EV_USUARIO 1
#define ev_USUARIO {ev_PULSAR_BOTON}

EVENTO_T evs_USUARIO[ev_NUM_EV_USUARIO] = {ev_PULSAR_BOTON}

#endif
