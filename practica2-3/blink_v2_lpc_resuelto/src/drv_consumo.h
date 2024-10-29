/* *****************************************************************************
 * P.H.2024: drv_cosumo.h
 * implementacion para cumplir el hal_tiempo.h
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 
#include <stdint.h>
 
#ifndef DRV_CONSUMO
#define DRV_CONSUMO

void drv_consumo_iniciar(uint32_t mon_wait, uint32_t mon_dormir);

void drv_consumo_esperar(void);

void drv_consumo_dormir(void);

#endif
