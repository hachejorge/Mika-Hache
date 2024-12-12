/* *****************************************************************************
 * P.H.2024: hal_consumo.c
 * especificaci�n de hal_consumo para gestionar el estado del procesador
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 
#ifndef HAL_CONSUMO
#define HAL_CONSUMO
 
#include <stdint.h>

// Inicia el m�dulo de consumo a nivel perif�rico
void hal_consumo_iniciar(void);

// Activa el bajo consumo de un procesador
void hal_consumo_esperar(void); 

// Apaga el procesador a nivel m�nimo
void hal_consumo_dormir(void);

#endif

