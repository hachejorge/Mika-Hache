
#include <nrf.h>
#include <board.h>
#include "drv_botones_ini.h"
#include "rt_FIFO.h"

/* *****************************************************************************
 * el pulsador genera un evento al cambiar de estado (toggle)
 * y genera una interrupcion de tipo: GPIOTE_IRQn
 * la RSI es GPIOTE_IRQHandler, definida en: arm_startup...
 */
void test_gpiote_button_int(){
  // Configure BUTTON3 to generate EVENTS_COMPARE[0] every 1000us.
  NRF_GPIOTE->CONFIG[3] = (GPIOTE_CONFIG_MODE_Event      << GPIOTE_CONFIG_MODE_Pos) |
                          (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) |
                          (BUTTON_3                << GPIOTE_CONFIG_PSEL_Pos);
	
	NRF_GPIOTE->INTENSET = 8;
	
	NVIC_EnableIRQ(GPIOTE_IRQn);
}

void GPIOTE_IRQHandler(){
  volatile uint32_t dummy;
	
	NRF_GPIOTE->EVENTS_IN[3] = 0; //puede tardar 5 ciclos... //ojo con optimizaciones

	dummy = NRF_GPIOTE->EVENTS_IN[3];
  dummy; 
					
	rt_FIFO_encolar(ev_PULSAR_BOTON,3);
	
}




/* *****************************************************************************
 * P.H.2024: hal_gpio_nrf.c
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 

#include "board.h"
#include "drv_botones.h"
#include "hal_ext_int.h"
#include "hal_gpio.h"
#include "rt_evento_t.h"
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
	//static BUTTON buttons[BUTTONS_NUMBER] = BUTTONS_LIST;
	static uint32_t button_list[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif

static void(*f_callback)(EVENTO_T id, uint32_t ms);		//puntero a funcion a llamar cuando salte la RSI (en modo irq)

static void llamar_f_callback(uint32_t pin) {
	#if BUTTONS_NUMBER > 0
		uint32_t id_boton;
		for(uint32_t i = 0; i < BUTTONS_NUMBER; i++) {
			if(pin == button_list[i]){
				id_boton = button_list[i];
			}
			hal_deshabilitar_int(button_list[i]);
		}
		f_callback(ev_PULSAR_BOTON, id_boton);
	#endif
}

void drv_botones_iniciar(void(*callback)(EVENTO_T id, uint32_t aux), EVENTO_T ev_PULSAR_BOTON, EVENTO_T ev_BOTON_RETARDN){
	#if BUTTONS_NUMBER > 0
		// Inicializar botones con el callback y otros par�metros
		hal_ext_int_iniciar(drv_botones_tratar);		
		f_callback = callback; 
		for (uint32_t i = 0; i < BUTTONS_NUMBER; ++i) 			{
			hal_gpio_sentido(button_list[i], HAL_GPIO_PIN_DIR_INPUT);
			hal_habilitar_int(button_list[i]);
		}

		// no se si estos dos suscribir tendrían que ir dentro de callback de butoton_list
		svc_GE_suscribir(ev_PULSAR_BOTON, drv_botones_tratar);
		svc_GE_suscribir(ev_BOTON_RETARDO, drv_botones_tratar);
	#endif //BUTTONS_NUMBER > 0	

}

void drv_botones_pulsado(){
#if BUTTONS_NUMBER > 0
        // Deshabilitamos las interrupciones de todos los botones para evitar conflictos
        for (uint32_t i = 0; i < BUTTONS_NUMBER; ++i) {
            hal_deshabilitar_int(button_list[i]);
        }
        
        // Llamar al callback con un evento de botón pulsado
        if (f_callback != NULL) {
            // Pasamos un evento indicando que se ha pulsado un botón
            // Utiliza un ID genérico o específico si es necesario
            f_callback(ev_PULSAR_BOTON, id_button); // '0' puede ser reemplazado por un identificador adecuado
        }
    #endif
}

void drv_botones_tratar(uint32_t auxiliar){
	switch (estado_actual){
	case e_reposo:
		if (evento == ev_PULSAR_BOTON) {
			uint32_t id_boton = auxiliar;
			
			// Usar el callback para encolar el ID del evento y datos auxiliares
			if (f_callback != NULL) {
				rt_FIFO_extraer(ev_PULSAR_BOTON, id_boton);
			}
			
			// Activar una alarma esporádica (Trp) para 30 ms
			svc_alarma_activar(svc_alarma_codificar(0, 30), ev_BOTON_RETARDO, id_boton);
			
			// Cambiar al estado 'e_entrando'
			estado_actual = e_entrando;
        }
		break;
	
	case e_entrando: 
		if (evento == ev_BOTON_RETARDO) {
			// Identificar la fuente si es necesario
			uint32_t id_boton = auxiliar;
			
			// Usar el callback para enviar ID_ev y auxdata
			if (f_callback != NULL) {
				rt_FIFO_extraer(ev_BOTON_RETARDO, id_boton);
			}
			
			// Programar una alarma periódica (Tep)
			svc_alarma_activar(svc_alarma_codificar(0, 50), ev_BOTON_RETARDO, id_boton);
			
			// Cambiar al estado 'e_esperando'
			estado_actual = e_esperando;
		}
		break;
	
	case e_esperando:
		// Manejar el evento ev_BOTON_RETARDO en el estado 'e_esperando'
            if (evento == ev_BOTON_RETARDO) {
                // Verificar si el botón se ha soltado
                if (botonsoltado?) {
                    // Programar una alarma esporádica (Trd)
                    (svc_alarma_codificar(0, 100), ev_BOTON_RETARDO, auxiliar);
                    
                    // Saltar al estado 'e_soltado'
                    estado_actual = e_soltado;
                }
            }
			 estado_actual = e_esperando;
		break;
	
	case e_soltado:
		 // Limpiar interrupciones pendientes
            hal_ext_int_iniciar();  // Suponiendo que hay una función para limpiar interrupciones
            
            // Habilitar las interrupciones nuevamente
            for (uint32_t i = 0; i < BUTTONS_NUMBER; ++i) {
                hal_habilitar_int(button_list[i]);
            }
            
            // Saltar al estado 'e_reposo'
            estado_actual = e_reposo;
		break;

	
	default:
		break;
	}
}

