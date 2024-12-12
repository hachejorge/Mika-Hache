#ifndef HAL_WDT
#define HAL_WDT

#include <stdint.h>

void hal_WDT_iniciar(uint32_t sec);

void hal_WDT_feed(void);

#endif
