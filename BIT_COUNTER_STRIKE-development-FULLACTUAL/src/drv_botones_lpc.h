#ifndef DRV_BOTONES_LPC
#define DRV_BOTONES_LPC

void init_eint0_button(void);

void EINT0_IRQHandler(void);

#endif

