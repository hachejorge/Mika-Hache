
#ifndef DRV_BUTTONS_INI
#define DRV_BUTTONS_INI



/* *****************************************************************************
 * el pulsador genera un evento al cambiar de estado (toggle)
 * y genera una interrupcion de tipo: GPIOTE_IRQn
 * la RSI es GPIOTE_IRQHandler, definida en: arm_startup...
 */
void test_gpiote_button_int(void);

void drv_botones_iniciar(void(*callback)(EVENTO_T id, uint32_t aux), EVENTO_T ev_PULSAR_BOTON, EVENTO_T ev_BOTON_RETARDO);

void drv_botones_tratar(uint32_t pin);

void drv_botones_pulsado(void);

#endif

