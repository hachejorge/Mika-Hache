/* *****************************************************************************
 * P.H.2024: drv_SC.c
 * Nekane Diaz Montoya   870795
 * Jorge Hernandez Aznar 872838
 */
 

#include "drv_SC.h"

// Número de procesos dentro de sección crítica
static uint32_t m_in_critical_region = 0;

uint32_t drv_SC_entrar_disable_irq(void) {
    __disable_irq();
    return(m_in_critical_region++);
}

void drv_SC_salir_enable_irq(void) {
    m_in_critical_region--;
    if (m_in_critical_region == 0) {
        __enable_irq();
    }
}
