#include "hal_ext_int.h"
#include <LPC21xx.h>  // Asegúrate de incluir el archivo adecuado para el microcontrolador LPC específico
#include <stdint.h>
#include "rt_evento_t.h"
#include "board.h"
#include "drv_botones.h"


// Declarar un puntero a la función de usuario que se ejecutará al activarse la interrupción
static void (*funcion_interrupcion_boton)(EVENTO_T evento, uint32_t aux);

// Manejador de Interrupción para EINT1 (asociada a P0.14)
void ISR_EINT1(void) __irq {
    if (funcion_interrupcion_boton) { 
        funcion_interrupcion_boton(ev_PULSAR_BOTON, BUTTON_1); // Llamar a la función registrada por el usuario
    }
    
    EXTINT |= (1 << 1);   // Limpiar la bandera de interrupción de EINT1
    VICVectAddr = 0;      // Acknowledge de la interrupción
}

// Manejador de Interrupción para EINT2 (asociada a P0.15)
void ISR_EINT2(void) __irq {
    if (funcion_interrupcion_boton) { 
        funcion_interrupcion_boton(ev_PULSAR_BOTON, BUTTON_2); // Llamar a la función registrada por el usuario
    }
    
    EXTINT |= (1 << 2);   // Limpiar la bandera de interrupción de EINT2
    VICVectAddr = 0;      // Acknowledge de la interrupción
}

// Manejador de Interrupción para EINT3 (asociada a P0.16)
void ISR_EINT3(void) __irq {
    if (funcion_interrupcion_boton) { 
        funcion_interrupcion_boton(ev_PULSAR_BOTON, BUTTON_3); // Llamar a la función registrada por el usuario
    }
    
    EXTINT |= (1 << 3);   // Limpiar la bandera de interrupción de EINT3
    VICVectAddr = 0;      // Acknowledge de la interrupción
}

// Inicialización de las interrupciones externas
void hal_ext_int_iniciar(void (*funcion_IRQ)(EVENTO_T evento, uint32_t aux)) {
    // Configurar la interrupción externa EINT1 para el pin P0.14
    // Configuración por nivel bajo (según los valores por defecto)
    // Configuración para activarse en bajo usando EXTPOLAR
    // P0.14 (pin asociado a EINT1)
    
    funcion_interrupcion_boton = funcion_IRQ;

    // Configurar el modo de interrupción de EINT1 como por nivel (0 en EXTMODE)
    EXTMODE &= ~(1 << 1);  // EINT1 en modo por nivel

    // Configurar la polaridad como activa en bajo (0 en EXTPOLAR)
    EXTPOLAR &= ~(1 << 1); // EINT1 activa en nivel bajo

    VICVectAddr1 = (unsigned long)ISR_EINT1;
    VICVectCntl1 = 0x20 | 14;        // Vector de EINT1

    // Habilitar la interrupción externa EINT1
    VICIntEnable |= (1 << 14);  // Bit 14 para EINT1 en el controlador de interrupciones

        PINSEL0 &= ~(0x03 << ( BUTTON_1* 2));   // Limpiar la configuración anterior
        PINSEL0 |= (0x01 << ( BUTTON_1 * 2));
}

// Habilitar interrupción externa en el pin especificado (para el LPC, esto está orientado a EINT1, EINT2 o EINT3)
void hal_habilitar_int(uint32_t pin) {
    if (pin == BUTTON_1 || pin == BUTTON_2 || pin == BUTTON_3) {
        // Configuración de EINT1,2,3 para detectar nivel bajo
        EXTINT |= (1 << 1);  // Limpiar cualquier interrupción pendiente en EINT1
        VICIntEnable |= (1 << (pin + 1)); // Habilitar EINT1 en el controlador de interrupciones
    }
}

// Deshabilitar interrupción externa en el pin especificado
void hal_deshabilitar_int(uint32_t pin) {
    if (pin == 14) {
        VICIntEnClr |= (1 << 15);  // Deshabilitar EINT1
    }
}

// Habilitar el modo de bajo consumo con despertar por interrupción
void hal_habilitar_despertar() {
    // Configuración específica del LPC para dormir profundo
    // Aquí podríamos poner el procesador en modo IDLE
    PCON |= 0x01;  // Poner el sistema en modo IDLE
}

// Deshabilitar el modo de bajo consumo
void hal_deshabilitar_despertar() {
    // Despertar el procesador; en LPC, cualquier interrupción debería salir del modo IDLE
    // No requiere acción adicional específica
}

