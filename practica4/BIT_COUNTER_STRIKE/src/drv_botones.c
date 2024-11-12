/* *****************************************************************************
 * P.H.2024: hal_gpio_nrf.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 

#include "board.h"
#include "drv_botones.h"
#include "hal_ext_int.h"
#include "hal_gpio.h"
#include "rt_GE.h"
#include "srv_alarm.h"
#include <stddef.h>

enum estados { 
	e_reposo,
	e_entrando,
	e_esperando,
	e_soltado
} estado_actual;

#if BUTTONS_NUMBER > 0
	static BUTTON buttons[BUTTONS_NUMBER] = BUTTONS_LIST;
	static uint32_t button_list[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif

void drv_botones_iniciar(void(*callback)(EVENTO_T id, uint32_t aux), EVENTO_T ev_PULSAR_BOTON, EVENTO_T ev_BOTON_RETARDO){
	#if BUTTONS_NUMBER > 0
		// Inicializar botones con el callback y otros par�metros
		
	
		for (uint32_t i = 0; i < BUTTONS_NUMBER; ++i) 			{
			hal_gpio_sentido(button_list[i], HAL_GPIO_PIN_DIR_INPUT);
			hal_habilitar_int(button_list[i]);
		}
		for (int i = 0; i < BUTTONS_NUMBER; i++) {
				buttons[i].id = i;
				buttons[i].pulsado = false;
				buttons[i].estado_actual = NULL;
				buttons[i].callback = callback; 		
		}
		// Configurar interrupciones externas en hal_ext_int
		hal_ext_int_iniciar();		
		// no se si estos dos suscribir tendrían que ir dentro de callback de butoton_list
		svc_GE_suscribir(ev_PULSAR_BOTON, drv_botones_tratar);
		svc_GE_suscribir(ev_BOTON_RETARDO, drv_botones_tratar);
	#endif //BUTTONS_NUMBER > 0	

}

void drv_botones_pulsado(){
	hal_deshabilitar_int();
}

void drv_botones_tratar(EVENTO_T evento, uint32_t auxiliar){
	switch (estado_actual){
	case e_reposo:
		
		svc_alarma_activar(svc_alarma_codificar(0,20), ev_BOTON_RETARDO, 0);
		estado_actual = e_entrando;
		break;
	
	case e_entrando: 
		
		estado_actual = e_esperando;
	
		break;
	
	case e_esperando:

		estado_actual = e_soltado;
		break;
	
	case e_soltado:

		estado_actual = e_reposo;
		break;

	
	default:
		break;
	}
}

