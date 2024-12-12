/* *****************************************************************************
 * P.H.2024: drv_SC.h
 * Nekane Diaz Montoya   870795
 * Jorge Hernandez Aznar 872838
 */
 
#ifndef DRV_SC
#define DRV_SC

#include <stdint.h>

// Desabilita las interrupciones, simulando una entrada en secci�n cr�tica
uint32_t drv_SC_entrar_disable_irq(void);

// Es un invocada por un proceso al salir de secci�n cr�tica, si no quedan m�s procesos en SC,
// vuelve a habilitar las IRQ's
void drv_SC_salir_enable_irq(void);

#endif
