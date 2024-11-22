#include "hal_ext_int.h"
#include "rt_evento_t.h"
#include <LPC210x.H> // Definiciones del hardware específico

// Defines para claridad en la asociación de pines a las interrupciones externas
#define PIN_EINT0 16 // EINT0 asociado al pin 14
#define PIN_EINT1 14 // EINT1 asociado al pin 15
#define PIN_EINT2 15 // EINT2 asociado al pin 16

// Callback global
static void (*ext_int_callback)(EVENTO_T evento, uint32_t pin);

/**
 * ISR para las interrupciones externas.
 */
void hal_ext_int_ISR(void) __irq {
    if (EXTINT & (1 << 0)) { // Verifica si la interrupción es de EINT0
        if (ext_int_callback) {
            ext_int_callback(ev_PULSAR_BOTON, PIN_EINT0); // Llama al callback con pin asociado
        }
        EXTINT = (1 << 0); // Limpia el flag de EINT0
    }
    if (EXTINT & (1 << 1)) { // Verifica si la interrupción es de EINT1
        if (ext_int_callback) {
            ext_int_callback(ev_PULSAR_BOTON, PIN_EINT1); // Llama al callback con pin asociado
        }
        EXTINT = (1 << 1); // Limpia el flag de EINT1
    }
    if (EXTINT & (1 << 2)) { // Verifica si la interrupción es de EINT2
        if (ext_int_callback) {
            ext_int_callback(ev_PULSAR_BOTON, PIN_EINT2); // Llama al callback con pin asociado
        }
        //EXTINT = (1 << 2);  // Limpia el flag de EINT2
				   EXTINT = 0x7; 
    }
    VICVectAddr = 0; // Señala fin de interrupción al VIC
}



/**
 * Inicializa el módulo de interrupciones externas y almacena la función de callback para interrupciones.
 */
void hal_ext_int_iniciar(void (*funcion_IRQ)(EVENTO_T evento, uint32_t pin)) {
    ext_int_callback = funcion_IRQ; // Guarda el callback

    // Configura los pines de interrupción externa
    PINSEL0 |= ((uint32_t)2U << 28); // Configura P0.14 como EINT0
    PINSEL0 |= ((uint32_t)2U << 30); // Configura P0.15 como EINT1
    PINSEL1 |= (1 << 0);  // Configura P0.16 como EINT2

    // Limpia flags de interrupción externa
    EXTINT = 0x7; // Borra las interrupciones pendientes en EINT0-EINT2

    // Configura vectores de interrupción en el VIC
    //VICIntSelect &= ~(1 << 14); // Configura EINT0 como IRQ
    //VICIntSelect &= ~(1 << 15); // Configura EINT1 como IRQ
    //VICIntSelect &= ~(1 << 16); // Configura EINT2 como IRQ

    VICVectAddr2 = (uint32_t)hal_ext_int_ISR; // Registra ISR en VIC
    VICVectCntl2 = 0x20 | 14;                // Habilita canal EINT0 en VIC

    VICVectAddr3 = (uint32_t)hal_ext_int_ISR; // Registra ISR en VIC
    VICVectCntl3 = 0x20 | 15;                // Habilita canal EINT1 en VIC

    VICVectAddr4 = (uint32_t)hal_ext_int_ISR; // Registra ISR en VIC
    VICVectCntl4 = 0x20 | 16;                // Habilita canal EINT2 en VIC
}

/**
 * Habilita las interrupciones en el pin especificado.
 */
void hal_habilitar_int(uint32_t pin) {
    if (pin == PIN_EINT0) {
        EXTINT &= ~(1 << 0); // Limpia el flag EINT0
        VICIntEnable |= (1 << PIN_EINT0); // Habilita EINT0
    } else if (pin == PIN_EINT1) {
        EXTINT &= ~(1 << 1); // Limpia el flag EINT1
        VICIntEnable |= (1 << PIN_EINT1); // Habilita EINT1
    } else if (pin == PIN_EINT2) {
        EXTINT &= ~(1 << 2); // Limpia el flag EINT2
        VICIntEnable |= (1 << PIN_EINT2); // Habilita EINT2
    }
}

/**
 * Deshabilita las interrupciones en el pin especificado.
 */
void hal_deshabilitar_int(uint32_t pin) {
    if (pin == PIN_EINT0) {
        VICIntEnClr |= (1 << PIN_EINT0); // Deshabilita EINT0
    } else if (pin == PIN_EINT1) {
        VICIntEnClr |= (1 << PIN_EINT1); // Deshabilita EINT1
    } else if (pin == PIN_EINT2) {
        VICIntEnClr |= (1 << PIN_EINT2); // Deshabilita EINT2
    }
}

/**
 * Configura el pin para que despierte el microcontrolador cuando el nivel sea bajo.
 */
void hal_habilitar_despertar(uint32_t pin) {
    if (pin == PIN_EINT0) {
        //EXTPOLAR &= ~(1 << 0); // Nivel bajo para EINT0
    } else if (pin == PIN_EINT1) {
        //EXTPOLAR &= ~(1 << 1); // Nivel bajo para EINT1
    } else if (pin == PIN_EINT2) {
        //EXTPOLAR &= ~(1 << 2); // Nivel bajo para EINT2
    }
}

/**
 * Configura el pin para que no despierte el microcontrolador.
 */
void hal_deshabilitar_despertar(uint32_t pin) {
    if (pin == PIN_EINT0) {
        //EXTPOLAR |= (1 << 0); // Nivel alto para EINT0
    } else if (pin == PIN_EINT1) {
        //EXTPOLAR |= (1 << 1); // Nivel alto para EINT1
    } else if (pin == PIN_EINT2) {
        //EXTPOLAR |= (1 << 2); // Nivel alto para EINT2
    }
}

/**
 * Limpia todos los canales y eventos en ellos.
 */
void hal_ext_int_limpiar(void) {
    EXTINT = 0x7; // Limpia flags de interrupción en EINT0-EINT2
    VICIntEnClr = (1 << 14) | (1 << 15) | (1 << 16); // Deshabilita EINT0, EINT1 y EINT2
}


