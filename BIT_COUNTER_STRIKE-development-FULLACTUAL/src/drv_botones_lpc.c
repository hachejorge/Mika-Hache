#include <LPC21xx.h>
#include "rt_FIFO.h"
#include "drv_leds.h"
#include "drv_botones_lpc.h"

#define BUTTON_PIN 16  // Pin asociado al bot�n (P0.16)

// Funci�n para inicializar la interrupci�n EINT0 asociada al bot�n
void init_eint0_button() {
    // Configurar el pin P0.16 como EINT0 (habilitar funci�n de interrupci�n externa)
    PINSEL1 &= ~(3 << 0);   // Limpiar bits 0 y 1
    PINSEL1 |=  (1 << 0);   // Configurar P0.16 como EINT0

    // Configurar el modo de disparo de la interrupci�n
    EXTMODE |=  (1 << 0);   // EINT0 en modo de borde
    EXTPOLAR &= ~(1 << 0);  // Disparar por flanco descendente

    // Habilitar la interrupci�n externa EINT0
    VICIntSelect &= ~(1 << 14);  // Configurar EINT0 como IRQ (no FIQ)
    VICVectAddr0 = (unsigned long)EINT0_IRQHandler;  // Direcci�n de la ISR
    VICVectCntl0 = (1 << 5) | 14;  // Habilitar el slot 0 para EINT0
    VICIntEnable = (1 << 14);  // Habilitar interrupci�n para EINT0
}

// ISR para EINT0
void EINT0_IRQHandler() {
    volatile uint32_t dummy;

    // Limpiar el evento de interrupci�n EINT0
    EXTINT |= (1 << 0);  // Limpiar el flag de EINT0

    // Leer estado para prevenir optimizaciones del compilador
    dummy = EXTINT & (1 << 0);
    dummy;

    // Invocar la funci�n para encolar el evento
    rt_FIFO_encolar(ev_PULSAR_BOTON, 3);
		//drv_led_conmutar(3);
    // Notificar al controlador de interrupciones que la ISR ha terminado
    VICVectAddr = 0;
}
