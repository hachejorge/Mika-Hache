#include <stdint.h>
#include "rt_evento_t.h"

#ifndef RT_GE
#define RT_GE

#define rt_GE_MAX_SUSCRITOS 4

typedef struct {
    void (*callbacks[rt_GE_MAX_SUSCRITOS])(uint32_t, uint32_t);
    uint8_t callbacks_usados;
} GE;


// Inicializar sus estructuras (estáticas)
// suscribir EVs usuario (lista rt_evento_t)
// activa alarma ev_INACTIVIDAD
void rt_GE_iniciar(uint32_t monitor);

void rt_GE_lanzador();

void svc_GE_suscribir(EVENTO_T evento, void(*f_callback)(uint32_t id, uint32_t aux));

void svc_GE_cancelar(EVENTO_T evento, void(*f_callback)(uint32_t id, uint32_t aux));

void rt_GE_tratar(EVENTO_T evento, uint32_t auxiliar);

#endif