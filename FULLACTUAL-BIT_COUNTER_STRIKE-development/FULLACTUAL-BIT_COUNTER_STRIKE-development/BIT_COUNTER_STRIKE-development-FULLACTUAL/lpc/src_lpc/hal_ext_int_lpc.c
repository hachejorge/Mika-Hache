
#include <LPC210x.h>
#include "rt_FIFO.h"
#include "drv_leds.h"
#include "drv_botones.h"

#define EINT0 14
#define EINT1 15
#define EINT2 16

static void (*callback_func)(EVENTO_T evento, uint32_t pin); // Función de callback


void EINT0_IRQHandler(void) {
    EXTINT |= (1 << 0); // Limpiar el flag de EINT0
    if (callback_func) callback_func(ev_PULSAR_BOTON, EINT2);
    VICVectAddr = 0; // Notificar al VIC que la ISR ha terminado
}

void EINT1_IRQHandler(void) {
    EXTINT |= (1 << 1); // Limpiar el flag de EINT1
    if (callback_func) callback_func(ev_PULSAR_BOTON, EINT0);
    VICVectAddr = 0; // Notificar al VIC que la ISR ha terminado
}

void EINT2_IRQHandler(void) {
    EXTINT |= (1 << 2); // Limpiar el flag de EINT2
    if (callback_func) callback_func(ev_PULSAR_BOTON, EINT1);
    VICVectAddr = 0; // Notificar al VIC que la ISR ha terminado
}

void hal_ext_int_iniciar(void (*funcion_IRQ)(EVENTO_T evento, uint32_t pin)) {
    callback_func = funcion_IRQ; // Almacenar la función de callback

    // Limpia y configura los registros de interrupciones externas
    EXTINT = 0x7;       // Limpiar todos los eventos pendientes para EINT0, EINT1, EINT2
    // Configuración básica del VIC para interrupciones externas
    VICIntEnClr = (1 << EINT0) | (1 << EINT1) | (1 << EINT2); // Deshabilitar interrupciones externas inicialmente
}

void hal_habilitar_int(uint32_t pin) {
 
// Configurar P0.14 como EINT0
    if (pin == EINT0) {
        PINSEL1 &= ~(3 << 0);   // Limpiar bits 0 y 1
        PINSEL1 |=  (1 << 0);   // Configurar P0.14 como EINT0
        VICVectAddr0 = (unsigned long)EINT0_IRQHandler; // Dirección de ISR
				VICVectCntl0 = (1 << 5) | 14;                  // Slot 0 para canal 14 (EINT0)
        VICIntEnable = (1 << 14);      // Habilitar EINT0
    }

    // Configurar P0.15 como EINT1
    if (pin == EINT1) {
        PINSEL1 &= ~(3 << 2);   // Limpiar bits 2 y 3
        PINSEL1 |=  (1 << 1);   // Configurar P0.15 como EINT1
        VICVectAddr1 = (unsigned long)EINT1_IRQHandler; // Dirección de ISR
				VICVectCntl1 = (1 << 5) | 15;                  // Slot 0 para canal 14 (EINT0)
        VICIntEnable = (1 << 15);      // Habilitar EINT1
    }

    // Configurar P0.16 como EINT2
    if (pin == EINT2) {
        PINSEL1 &= ~(3 << 4);   // Limpiar bits 4 y 5
        PINSEL1 |=  (1 << 4);   // Configurar P0.16 como EINT2
        VICVectAddr2 = (unsigned long)EINT2_IRQHandler; // Dirección de ISR
        VICVectCntl2 = (1 << 5) | 16;  // Habilitar el slot 2 para EINT2
        VICIntEnable = (1 << 16);      // Habilitar EINT2
    }

}

void hal_deshabilitar_int(uint32_t pin) {
	if (pin == EINT0) {
        EXTINT |= (1 << 0);    // Limpiar la bandera de interrupción de EINT0
    }
    if (pin == EINT1) {
        EXTINT |= (1 << 1);    // Limpiar la bandera de interrupción de EINT1
    }
    if (pin == EINT2) {
        EXTINT |= (1 << 2);    // Limpiar la bandera de interrupción de EINT2
    }
}


void hal_habilitar_despertar(void){
	EXTWAKE |= (1 << 0) | (1 << 1) | (1 << 2);
}

void hal_deshabilitar_despertar(void){
	EXTWAKE |= ~(1 << 0) | ~(1 << 1) | ~(1 << 2);
}

// Función que limpia las interrupciones pendientes
void hal_ext_int_limpiar(void) {
    EXTINT = 0x7;  // Limpia los flags de EINT0, EINT1 y EINT2
}








