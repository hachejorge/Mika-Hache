/* *****************************************************************************
 * P.H.2024: hal_gpio_nrf.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */

#include "hal_gpio.h"
#include <nrf.h>

/**
 * Permite emplear el GPIO y debe ser invocada antes
 * de poder llamar al resto de funciones de la biblioteca.
 * re-configura todos los pines como de entrada (para evitar cortocircuitos)
 */
void hal_gpio_iniciar(void) {
	// Reiniciamos los pines todos como salida (igual al reset)
	NRF_GPIO->DIR = 0x0; 
}

/* *****************************************************************************
 * Acceso a los GPIOs
 *
 * gpio_inicial indica el primer bit con el que operar.
 * num_bits indica cuántos bits con los que queremos operar.
 */

/**
 * Los bits indicados se configuran como
 * entrada o salida según la dirección.
 */
void hal_gpio_sentido_n(HAL_GPIO_PIN_T gpio_inicial, uint8_t num_bits, hal_gpio_pin_dir_t direccion) {
		uint32_t masc = ((1 << num_bits) - 1) << gpio_inicial;
		for(int i = 0; i < 32; i++) {
				if(masc & (1<<i)) {
						if(direccion == HAL_GPIO_PIN_DIR_INPUT) {
								NRF_GPIO->PIN_CNF[i] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                              	       (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
									   (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                              	       (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) |
                              	       (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
						}
						else if(direccion == HAL_GPIO_PIN_DIR_OUTPUT){
								NRF_GPIO->PIN_CNF[i] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
                              	       (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                                       (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                              	       (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
                              	       (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
						}
				}
		}
}

/**
 * La función devuelve un entero con el valor de los bits indicados.
 * Ejemplo:
 *		- valor de los pines: 0x0F0FAFF0
 *		- bit_inicial: 12 num_bits: 4
 *		- valor que retorna la función: 10 (lee los 4 bits 12-15)
 */
uint32_t hal_gpio_leer_n(HAL_GPIO_PIN_T gpio_inicial, uint8_t num_bits)
{
	uint32_t masc = ((1 << num_bits) - 1) << gpio_inicial; // El 1 es desplazado tantas posiciones como num_bits

	return (NRF_GPIO->IN & masc) >> gpio_inicial;
}

/**
 * Escribe en los bits indicados el valor
 * (si valor no puede representarse en los bits indicados,
 *  se escribirá los num_bits menos significativos a partir del inicial).
 */
void hal_gpio_escribir_n(HAL_GPIO_PIN_T bit_inicial, uint8_t num_bits, uint32_t valor)
{
	uint32_t masc_value = (valor & ((1 << num_bits) - 1)) << bit_inicial;
	uint32_t masc = ((1 << num_bits) - 1) << bit_inicial; // Crea una máscara de los bits que se quieren modificar
	uint32_t temp = NRF_GPIO->IN & ~masc; // // Leer el estado actual de los pines del GPIO, aplicando una máscara inversa (~masc)
	NRF_GPIO->OUT = temp | masc_value; // / Escribir el valor proporcionado en los bits correspondientes, combinando el estado anterior con el nuevo valor en los bits deseados.
}


/* *****************************************************************************
 * Acceso a los GPIOs
 *
 * a gpio unico (optimizar accesos)
 */

/**
 * El gpio se configuran como entrada o salida según la dirección.
 */
void hal_gpio_sentido(HAL_GPIO_PIN_T gpio, hal_gpio_pin_dir_t direccion)
{
	if (direccion == HAL_GPIO_PIN_DIR_INPUT)
	{
		NRF_GPIO->PIN_CNF[gpio] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                              (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
															(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                              (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) |
                              (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
	}
	else if (direccion == HAL_GPIO_PIN_DIR_OUTPUT)
	{
		NRF_GPIO->PIN_CNF[gpio] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
                              (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                              (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                              (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
                              (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
	}
}

/**
 * La funci�n devuelve un entero (bool) con el valor de los bits indicados.
 */
uint32_t hal_gpio_leer(HAL_GPIO_PIN_T gpio){
	uint32_t masc = (1UL << gpio); //  Genera un valor con solo el bit correspondiente al número del pin `gpio` en alto.
	return ((NRF_GPIO->IN & masc)!=0); // si el valor devuelto es 1 significa que el pin esta en alto, en caso contrario (0) que está bajo
}


/**
 * Escribe en el gpio el valor
 */
void hal_gpio_escribir(HAL_GPIO_PIN_T gpio, uint32_t valor){
	uint32_t masc = (1UL << gpio); //  Genera un valor con solo el bit correspondiente al número del pin `gpio` en alto.

	// Si el valor a escribir es 0, se desactiva el pin (se limpia el bit correspondiente en el registro OUTCLR).
    // Si el valor es 0, se ejecuta NRF_GPIO->OUTCLR = masc, que limpia (pone en bajo) el pin especificado por la máscara.
	if ((valor & 0x01) == 0) NRF_GPIO->OUTCLR = masc;
	else NRF_GPIO->OUTSET = masc;
}



