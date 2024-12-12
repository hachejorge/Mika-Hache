/* *****************************************************************************
 * P.H.2024: drv_cosumo.h
 * especificación del módulo drv_consumo
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 
#include <stdint.h>

 
#ifndef DRV_CONSUMO
#define DRV_CONSUMO

/* Inicializa el modulo de consumo, guardando los valores de los monitores a marcar
*/
void drv_consumo_iniciar(uint32_t mon_wait, uint32_t mon_dormir);

/* Activa el modo de bajo consumo del procesador
*/
void drv_consumo_esperar(void);

/* Se desactiva el procesador teniendo el mínimo consumo posible
*/
void drv_consumo_dormir(void);

#endif
