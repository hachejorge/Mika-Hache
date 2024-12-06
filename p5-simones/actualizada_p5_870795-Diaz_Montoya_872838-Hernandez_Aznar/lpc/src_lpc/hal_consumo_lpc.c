/* *****************************************************************************
 * P.H.2024: hal_consumo_lpc.c
 * implementación del módulo hal_consumo para el simulador LPC2105
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */

#include "hal_consumo.h"
#include <LPC210x.H>                       /* LPC210x definitions */

//definida en Startup.s
extern void switch_to_PLL(void);

/* inicia el hal de consumo */
void hal_consumo_iniciar(void) {
}

/* pone al procesador en estado de espera para reducir su consumo */
void hal_consumo_esperar(void)  {
  PCON |= 0x01; 
}

/* duerme al procesador para minimizar su consumo */
void hal_consumo_dormir(void)  {
  EXTWAKE = 7;  // Configurar para que EINT0, EINT1, EINT2 despierten el procesador
  PCON |= 0x02; // Entrar en modo Power-down
  // Aquí el procesador entrará en modo bajo consumo y se detendrá hasta una interrupción.
  switch_to_PLL(); // Reconfigurar el PLL al despertar
}


