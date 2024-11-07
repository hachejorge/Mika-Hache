/* *****************************************************************************
 * P.H.2024: hal_gpio_nrf.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 

#include "board_nrf52840dk.h"
#include "drv_botones.h"
#include <stddef.h>

#if BUTTONS_NUMBER > 0
	static BUTTON button_list[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif


uint32_t drv_leds_iniciar(EVENTO_T evento, void(*f_callback)(uint32_t id, uint32_t aux)){
	#if BUTTONS_NUMBER > 0
			 // Inicializar botones con el callback y otros parámetros
		
			for (int i = 0; i < BUTTONS_NUMBER; i++) {
					button_list[i].id = i;
					button_list[i].pulsado = false;
					button_list[i].estado_actual = NULL;
					button_list[i].callback = callback;
			}
			// Configurar interrupciones externas en hal_ext_int
			hal_ext_int_iniciar();		
  #endif //BUTTONS_NUMBER > 0	
	
	return BUTTONS_NUMBER;  //definido en board_xxx.h en cada placa... 
}


