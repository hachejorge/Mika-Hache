
#include <LPC21xx.h>
#include "rt_FIFO.h"
#include "drv_leds.h"
#include "drv_botones.h"

#define EINT0 14
#define EINT1 15
#define EINT2 16

static void (*callback_func)(EVENTO_T evento, uint32_t pin); // Función de callback

void hal_ext_int_limpiar(void) {
    EXTINT = 0x7;  // Limpia los flags de EINT0, EINT1 y EINT2
}

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
    EXTMODE = 0x0;      // Configura todas las interrupciones en nivel por defecto
    EXTPOLAR = 0x0;     // Configura polaridad descendente por defecto

    // Configuración básica del VIC para interrupciones externas
    VICIntEnClr = (1 << EINT0) | (1 << EINT1) | (1 << EINT2); // Deshabilitar interrupciones externas inicialmente
}

void hal_habilitar_int(uint32_t pin) {
    switch (pin) {
        case EINT1: // Configurar EINT1
            PINSEL1 &= ~(3 << 2);   // Limpiar bits 2 y 3
            PINSEL1 |=  (1 << 2);   // Configurar P0.14 como EINT1

            EXTMODE |=  (1 << 1);   // EINT1 en modo de borde
            EXTPOLAR &= ~(1 << 1);  // Disparar por flanco descendente

            VICIntSelect &= ~(1 << 15);           // Configurar como IRQ
            VICVectAddr1 = (unsigned long)EINT1_IRQHandler; // Dirección de ISR
            VICVectCntl1 = (1 << 5) | 15;        // Habilitar el slot 1 para EINT1
            VICIntEnable = (1 << 15);            // Habilitar EINT1
            break;

        case EINT2: // Configurar EINT2
            PINSEL1 &= ~(3 << 4);   // Limpiar bits 4 y 5
            PINSEL1 |=  (1 << 4);   // Configurar P0.15 como EINT2

            EXTMODE |=  (1 << 2);   // EINT2 en modo de borde
            EXTPOLAR &= ~(1 << 2);  // Disparar por flanco descendente

            VICIntSelect &= ~(1 << 16);           // Configurar como IRQ
            VICVectAddr2 = (unsigned long)EINT2_IRQHandler; // Dirección de ISR
            VICVectCntl2 = (1 << 5) | 16;        // Habilitar el slot 2 para EINT2
            VICIntEnable = (1 << 16);            // Habilitar EINT2
            break;

        case EINT0: // Configurar EINT0
            PINSEL1 &= ~(3 << 0);   // Limpiar bits 0 y 1
            PINSEL1 |=  (1 << 0);   // Configurar P0.16 como EINT0

            EXTMODE |=  (1 << 0);   // EINT0 en modo de borde
            EXTPOLAR &= ~(1 << 0);  // Disparar por flanco descendente

            VICIntSelect &= ~(1 << 14);           // Configurar como IRQ
            VICVectAddr0 = (unsigned long)EINT0_IRQHandler; // Dirección de ISR
            VICVectCntl0 = (1 << 5) | 14;        // Habilitar el slot 0 para EINT0
            VICIntEnable = (1 << 14);            // Habilitar EINT0
            break;
    }
}

void hal_deshabilitar_int(uint32_t pin) {
    /*switch (pin) {
        case BUTTON_1:
            EXTINT |= (1 << 1);       // Limpiar el flag de EINT1
            VICIntEnClr = (1 << 15);  // Deshabilitar EINT1 en el VIC
            VICVectAddr1 = 0;         // Limpiar el vector
            break;

        case BUTTON_2:
            EXTINT |= (1 << 2);       // Limpiar el flag de EINT2
            VICIntEnClr = (1 << 16);  // Deshabilitar EINT2 en el VIC
            VICVectAddr2 = 0;         // Limpiar el vector
            break;

        case BUTTON_3:
            EXTINT |= (1 << 0);       // Limpiar el flag de EINT0
            VICIntEnClr = (1 << 14);  // Deshabilitar EINT0 en el VIC
            VICVectAddr0 = 0;         // Limpiar el vector
            break;
    }*/
}



