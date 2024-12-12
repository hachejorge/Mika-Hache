/* *****************************************************************************
 * P.H.2024: drv_SC.h
 * Nekane Diaz Montoya   870795
 * Jorge Hernandez Aznar 872838
 */
 
#ifndef DRV_SC
#define DRV_SC

#include <stdint.h>

// Desabilita las interrupciones, simulando una entrada en sección crítica
uint32_t drv_SC_entrar_disable_irq(void);

// Es un invocada por un proceso al salir de sección crítica, si no quedan más procesos en SC,
// vuelve a habilitar las IRQ's
void drv_SC_salir_enable_irq(void);

#endif
