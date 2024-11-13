#ifndef HAL_EXT_INT
#define HAL_EXT_INT

#include <stdint.h>

void hal_ext_int_iniciar(void);

void hal_habilitar_int(uint32_t pin);

void hal_deshabilitar_int(uint32_t pin);

void hal_habilitar_despertar(void);

void hal_deshabilitar_despertar(void);

#endif
