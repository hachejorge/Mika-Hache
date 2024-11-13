#include "hal_consumo.h"
#include <LPC210x.H>                       /* LPC210x definitions */

//definida en Startup.s
extern void switch_to_PLL(void);

/* inicia el hal de consumo */
void hal_consumo_iniciar(void) {
}

/* pone al procesador en estado de espera para reducir su consumo */
void hal_consumo_esperar(void)  {
  EXTWAKE = (1 << 0) | (1 << 1) | (1 << 2); 		// EXTINT0,1,2 will awake the processor
	PCON |= 0x01; 
}

/* duerme al procesador para minimizar su consumo */
void hal_consumo_dormir(void)  {
  EXTWAKE = 7;  // Configurar para que EINT0, EINT1, EINT2 despierten el procesador
  PCON |= 0x02; // Entrar en modo Power-down
  __asm("NOP"); // Instrucción de espera para asegurarse de que el sistema entra en modo Power-down
  // Aquí el procesador entrará en modo bajo consumo y se detendrá hasta una interrupción.
  switch_to_PLL(); // Reconfigurar el PLL al despertar
}


