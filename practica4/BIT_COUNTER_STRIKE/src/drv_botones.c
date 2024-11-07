/* *****************************************************************************
 * P.H.2024: hal_gpio_nrf.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 

#include "board.h"
#include "drv_botones.h"
#include "hal_ext_int.h"
#include <stddef.h>

enum estados { 
	e_reposo,
	e_entrando,
	e_esperando,
	e_soltando
} estado_actual;

#if BUTTONS_NUMBER > 0
	static BUTTON button_list[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif

void drv_botones_iniciar(void(*rt_FIFO_encolar)(uint32_t id, uint32_t aux), EVENTO_T ev_PULSAR_BOTON, EVENTO_T ev_BOTON_RETARDO){
	#if BUTTONS_NUMBER > 0
		// Inicializar botones con el callback y otros par�metros
	
		for (int i = 0; i < BUTTONS_NUMBER; i++) {
				button_list[i].id = i;
				button_list[i].pulsado = false;
				button_list[i].estado_actual = NULL;
				button_list[i].callback = rt_FIFO_encolar; 			//No estoy segura de que haya que pomer esta funcion aqui???????
		}
		// Configurar interrupciones externas en hal_ext_int
		hal_ext_int_iniciar();		
	#endif //BUTTONS_NUMBER > 0	
	// no se si estos dos suscribir tendrían que ir dentro de callback de butoton_list
	svc_GE_suscribir(ev_PULSAR_BOTON, drv_botones_tratar);
	svc_GE_suscribir(ev_BOTON_RETARDO, drv_botones_tratar);

}

void drv_botones_tratar(EVENTO_T evento, uint32_t auxiliar){
	switch (estado_actual){
	case e_entrando:
		
		break;
	case e_esperando: 
		
		break;
	case e_reposo:
		
		break;
	case e_soltando:
		
		break;
	default:
		break;
	}
}

