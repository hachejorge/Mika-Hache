#include "hal_ext_int.h"
#include <nrf.h>
#include "rt_fifo.h"

#define NUM_CANALES 8

static void (*callback_func)(EVENTO_T evento, uint32_t pin); // Función de callback almacenada

static int32_t pin_a_canal[NUM_CANALES];
static uint32_t canales_ocupados;

// Devuelve el primer canal libre sin pin asignado, en caso de que no encuentre ninguno libre devuelve -1
int32_t encontrar_canal_libre(){
		for(int i = 0; i < NUM_CANALES; i++){
				if(pin_a_canal[i] == -1){ // si el pin está libre, es decir tiene un -1 guardado, devuelve el chanel
					return i; 
				} 
		}
		return -1;
}

void f_callbackIRQ(uint32_t pin){
		callback_func(ev_PULSAR_BOTON,pin); 
}

/**
 * Inicializa el módulo y almacena la función de callback para interrupciones.
 * funcion_IRQ Puntero a la función que se ejecutará en la interrupción.
 */
void hal_ext_int_iniciar(void (*funcion_IRQ)(EVENTO_T evento, uint32_t pin)) {
		callback_func = funcion_IRQ;
		
		canales_ocupados = 0;
		// Limpiamos todos los canales antes de empezar 
		for(int i = 0; i < NUM_CANALES; i++){
		// Limpiar configuración del canal
			NRF_GPIOTE->CONFIG[i] = 0;
				// Deshabilitar interrupciones en el canal
				NRF_GPIOTE->INTENCLR = (1 << i);
				// Ponemos a 0 el canal
				pin_a_canal[i] = -1;

	}
}

/**
 * Habilita las interrupciones en el pin especificado.
 * Pin en el que se habilitarán las interrupciones.
 */
void hal_habilitar_int(uint32_t pin) {
		// Primero se verifica si el canal ya está configurado
		for (int i = 0; i < NUM_CANALES; i++) {
				if (pin_a_canal[i] == (int32_t)pin) {
						return; // El pin ya está configurado
				}
		}    

		if (canales_ocupados < NUM_CANALES) {
				int32_t canal = encontrar_canal_libre();

				// Configurar GPIOTE para generar eventos en el pin, configuramos que se produzca una interrupción cada vez que el valor cambie
				NRF_GPIOTE->CONFIG[canal] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
																		(GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos) |
																		(pin << GPIOTE_CONFIG_PSEL_Pos);

				// Habilitar interrupciones en el canal
				NRF_GPIOTE->INTENSET = (1 << canal);

				// Habilitar la IRQ en el NVIC
				NVIC_EnableIRQ(GPIOTE_IRQn);

				// Actualizar la estructura de canales
				pin_a_canal[canal] = pin;
				canales_ocupados++;
		}
}

/**
 * Deshabilita las interrupciones en el pin especificado.
 * Pin en el que se deshabilitarán las interrupciones.
 */
void hal_deshabilitar_int(uint32_t pin) {
		for(int i = 0; i < NUM_CANALES; i++) {
				if(pin_a_canal[i] == pin){
						
						// Deshabilitar interrupciones en el canal
						NRF_GPIOTE->INTENCLR = (1 << i);

						// Limpiar configuración del canal
						NRF_GPIOTE->CONFIG[i] = 0;

						// Actualizar la estructura de canales
						pin_a_canal[i] = -1;
						canales_ocupados--;

						// Deshabilitar la IRQ en el NVIC si no quedan canales habilitados
						if (canales_ocupados == 0) {
								NVIC_DisableIRQ(GPIOTE_IRQn);
						}
				}
		}
}


/**
 * Manejador de interrupciones GPIOTE. Esta rutinna de interrupción se llamará a f_callbackIRQ
 */
void GPIOTE_IRQHandler(void) {
		for (uint32_t canal = 0; canal < NUM_CANALES; canal++) {
				if (NRF_GPIOTE->EVENTS_IN[canal]) {
						NRF_GPIOTE->EVENTS_IN[canal] = 0; // Limpiar evento
	
						f_callbackIRQ(pin_a_canal[canal]);
				}
		}
}

/**
* Limpia todos los canales, los eventos en ellos y deshabilita las interrupciones en el canal
 */

void hal_ext_int_limpiar(){
		// De forma similiar a iniciar se limpian todos los canales
		canales_ocupados = 0;
		// Limpiamos todos los canales antes de empezar 
		for(int i = 0; i < NUM_CANALES; i++){
				// Se limpia un posibles evento en el canal
			NRF_GPIOTE->EVENTS_IN[i] = 0;
				// Limpiar configuración del canal
			NRF_GPIOTE->CONFIG[i] = 0;
				// Deshabilitar interrupciones en el canal
				NRF_GPIOTE->INTENCLR = (1 << i);
				// Ponemos a 0 el canal
				pin_a_canal[i] = -1;
	}
}

/*
* Configura el pin para que despierte el microcontrolador cuando el nivel sea bajo (0 lógico).
*/
void hal_habilitar_despertar(uint32_t pin){
		NRF_GPIO->PIN_CNF[pin] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);	
}

/*
* Deshabilita la detección de eventos en el pin, evitando que despierte al microcontrolador.
*/
void hal_deshabilitar_despertar(uint32_t pin) {
		NRF_GPIO->PIN_CNF[pin] |= (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);	
}


