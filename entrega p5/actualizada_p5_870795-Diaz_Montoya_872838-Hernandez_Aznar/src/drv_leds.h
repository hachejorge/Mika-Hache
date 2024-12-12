/* *****************************************************************************
 * P.H.2024: Driver/Manejador de los Leds
 * suministra los servicios de iniciar, encender, apagar, conmutar... independientemente del hardware
 */

#ifndef DRV_LEDS
#define DRV_LEDS

#include <stdint.h>

/**
 * inicializa los led, los deja apagados y devuelve el numero de leds disponibles en la plataforma
 */
uint32_t drv_leds_iniciar(void);

/**
 * enciende el led id, si id es cero ... no enciende ninguno?, todos? decidis vosotros
 */
void drv_led_encender(uint32_t id);

/**
 * funcion complementaria a encender
 */
void drv_led_apagar(uint32_t id);

/**
 * conmuta el led de on a off y viceversa
 */
void drv_led_conmutar(uint32_t id);

/**
 * enciende todos los leds
 */
void drv_leds_encender_todos(void);

/**
 * apaga todos los leds
 */
void drv_leds_apagar_todos(void);

/**
 * conmuta todos los leds
 */
void drv_leds_conmutar_todos(void);

#if 0
/**
 * otras funciones de alto nivel que podrian sernos utiles
 * podeis realizarlas opcionalmente o si las usais en vuestra app
 */
uint32_t drv_led_estado(uint32_t id);


#endif

#endif
