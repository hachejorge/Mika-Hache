#include "hal_ext_int.h"
#include <LPC210x.h>  // Asegúrate de incluir el archivo adecuado para el microcontrolador LPC específico

// Inicialización de las interrupciones externas
void hal_ext_int_iniciar() {
    // Configurar la interrupción externa EINT1 para el pin P0.14
    // Configuración por nivel bajo (según los valores por defecto)
    // Configuración para activarse en bajo usando EXTPOLAR
    // P0.14 (pin asociado a EINT1)
    
    // Configurar el modo de interrupción de EINT1 como por nivel (0 en EXTMODE)
    EXTMODE &= ~(1 << 1);  // EINT1 en modo por nivel

    // Configurar la polaridad como activa en bajo (0 en EXTPOLAR)
    EXTPOLAR &= ~(1 << 1); // EINT1 activa en nivel bajo

    // Habilitar la interrupción externa EINT1
    VICIntEnable |= (1 << 15);  // Bit 15 para EINT1 en el controlador de interrupciones
}

// Habilitar interrupción externa en el pin especificado (para el LPC, esto está orientado a EINT1/P0.14)
void hal_habilitar_int(uint32_t pin) {
    if (pin == 14) {
        // Configuración de EINT1 para detectar nivel bajo
        EXTINT |= (1 << 1);  // Limpiar cualquier interrupción pendiente en EINT1
        VICIntEnable |= (1 << 15); // Habilitar EINT1 en el controlador de interrupciones
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
