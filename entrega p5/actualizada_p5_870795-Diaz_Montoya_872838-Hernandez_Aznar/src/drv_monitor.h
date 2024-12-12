/* *****************************************************************************
 * P.H.2024: drv_monitor.h
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 
#ifndef DRV_MONITOR
#define DRV_MONITOR

#include <stdint.h>

// Devuelve el número de monitores
uint32_t drv_monitor_iniciar(void);

//Marcamos el monitor como monitor activo
void drv_monitor_marcar(uint32_t id);

//Desmarcamos el monitor como monitor en estado no activo
void drv_monitor_desmarcar(uint32_t id);

#endif

