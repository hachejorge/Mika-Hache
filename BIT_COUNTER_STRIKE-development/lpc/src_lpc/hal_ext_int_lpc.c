#include <LPC21xx.h>
#include "rt_FIFO.h"
#include "drv_leds.h"
#include "drv_botones_lpc.h"


void hal_ext_int_iniciar(void (*funcion_IRQ)(uint32_t pin)){

}
void hal_habilitar_int(uint32_t pin);

void hal_deshabilitar_int(uint32_t pin);

void hal_habilitar_despertar(void);

void hal_deshabilitar_despertar(void);