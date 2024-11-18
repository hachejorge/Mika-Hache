#ifndef HAL_EXT_INT
#define HAL_EXT_INT

#include "rt_evento_t.h"
#include <stdint.h>

void hal_ext_int_iniciar(void (*funcion_IRQ)(uint32_t pin));

void hal_habilitar_int(uint32_t pin);

void hal_deshabilitar_int(uint32_t pin);

void hal_habilitar_despertar(void);

void hal_deshabilitar_despertar(void);

#endif
