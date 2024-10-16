/* *****************************************************************************
 * P.H.2024: TODO
 */

#include "hal_gpio_nrf.h"
#include "hal_gpio.h"
#include <nrf.h>

/**
 * Permite emplear el GPIO y debe ser invocada antes
 * de poder llamar al resto de funciones de la biblioteca.
 * re-configura todos los pines como de entrada (para evitar cortocircuitos)
 */
void hal_gpio_iniciar(void)
{
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
void hal_gpio_sentido_n(HAL_GPIO_PIN_T gpio_inicial, uint8_t num_bits, hal_gpio_pin_dir_t direccion)
{
	uint32_t masc = ((1 << num_bits) - 1) << gpio_inicial;
	if (direccion == HAL_GPIO_PIN_DIR_INPUT)
	{
		//for(int i = 0; i < 32; i++){
			//NRF_GPIO->PIN_CNF[i] = (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos);
		//}
		NRF_GPIO->DIR = NRF_GPIO->DIR & ~masc;
	}
	else if (direccion == HAL_GPIO_PIN_DIR_OUTPUT)
	{
		for(int i = 0; i < 32; i++){
			NRF_GPIO->PIN_CNF[i] = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
                              (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                              (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                              (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
                              (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
		}
		//NRF_GPIO->DIR = NRF_GPIO->DIR | masc;
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
	uint32_t masc = ((1 << num_bits) - 1) << gpio_inicial;

	return (NRF_GPIO->IN & masc) >> gpio_inicial;
	// IOPIN : GPIO Port Pin value register. Contiene el estado de los
	// puertos pines configurados independientemente de la direccion.
}

/**
 * Escribe en los bits indicados el valor
 * (si valor no puede representarse en los bits indicados,
 *  se escribirá los num_bits menos significativos a partir del inicial).
 */
void hal_gpio_escribir_n(HAL_GPIO_PIN_T bit_inicial, uint8_t num_bits, uint32_t valor)
{
	uint32_t masc_value = (valor & ((1 << num_bits) - 1)) << bit_inicial;
	uint32_t masc = ((1 << num_bits) - 1) << bit_inicial;
	uint32_t temp = NRF_GPIO->IN & ~masc; //NRF_GPIO->OUT & ~masc;
	NRF_GPIO->OUT = temp | masc_value;
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
	uint32_t masc = (1UL << gpio);
	if (direccion == HAL_GPIO_PIN_DIR_INPUT)
	{
		NRF_GPIO->DIR = NRF_GPIO->DIR & ~masc;
	}
	else if (direccion == HAL_GPIO_PIN_DIR_OUTPUT)
	{
		NRF_GPIO->DIR = NRF_GPIO->DIR | masc;
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
	uint32_t masc = (1UL << gpio);
	return ((NRF_GPIO->IN & masc)!=0);
}


/**
 * Escribe en el gpio el valor
 */
void hal_gpio_escribir(HAL_GPIO_PIN_T gpio, uint32_t valor){
	uint32_t masc = (1UL << gpio);
	
	if ((valor & 0x01) == 0) NRF_GPIO->OUTCLR = masc;
	else NRF_GPIO->OUTSET = masc;
}



