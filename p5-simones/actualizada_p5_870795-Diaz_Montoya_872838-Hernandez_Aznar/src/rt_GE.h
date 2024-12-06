#include <stdint.h>
#include "rt_evento_t.h"

#ifndef RT_GE
#define RT_GE



// Inicializar sus estructuras (estáticas)
// suscribir EVs usuario (lista rt_evento_t)
// activa alarma ev_INACTIVIDAD
void rt_GE_iniciar(uint32_t monitor);

// Extrae eventos de la fifo y llama a su callback
void rt_GE_lanzador(void);

// Si hay espacio en el GE, se suscribe una nueva función (callback) a ese evento
void svc_GE_suscribir(EVENTO_T evento, void(*f_callback)(EVENTO_T id, uint32_t aux));

// Si hay espacios vacíos, disminuimos el número de callbacj asociados a ese evento
void svc_GE_cancelar(EVENTO_T evento, void(*f_callback)(EVENTO_T id, uint32_t aux));

#endif

