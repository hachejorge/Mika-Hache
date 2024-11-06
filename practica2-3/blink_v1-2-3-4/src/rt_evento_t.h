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
} EVENTO_T;			    //mapea a uint32_t

#define EVENT_TYPES 3

#endif

