/* *****************************************************************************
 * P.H.2024: drv_botones.c
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

#define TRP 10 // Tiempo de rebote de entrada
#define TEP 10 // Periodo para comprobar si se ha soltado el botón
#define TRD 10 // Tiempo de rebote de salida


// Estados posibles del flujo habitual de un botón
enum estados { 
	e_reposo,
	e_entrando,
	e_esperando,
	e_soltado
} estado_actual;

#if BUTTONS_NUMBER > 0
	static uint32_t button_list[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif

// OPERACIONES PRIVADAS

// Devuelve el botón equivalente a un pin
uint32_t boton_equivalente_pin(uint32_t pin) {
	#if BUTTONS_NUMBER > 0
		for(int i = 0; i < BUTTONS_NUMBER; i++){
			if(pin == button_list[i]){
				return i+1;
			}
		}
	#endif
	return 0;
}

// Devuelve true si el pin seleccionado está pulsado, false en caso contrario
bool boton_pulsado(uint32_t pin) {
	if(hal_gpio_leer(pin) == BUTTONS_ACTIVE_STATE){
		return true;
	}
    return false;
}

// OPERACIONES PÚBLICAS

static void(*f_callback)(EVENTO_T id, uint32_t aux);		//puntero a funcion a llamar cuando salte la RSI (en modo irq)

void f_callback_drv(EVENTO_T id, uint32_t aux){
	f_callback(id,boton_equivalente_pin(aux));
}


// Inicia el módulo 
void drv_botones_iniciar(void(*callback)(EVENTO_T id, uint32_t aux), EVENTO_T ev_PULSAR_BOTON, EVENTO_T ev_BOTON_RETARD0){
	#if BUTTONS_NUMBER > 0
		// Inicializar botones con el callback y otros par�metros
		hal_ext_int_iniciar(f_callback_drv);		
		estado_actual = e_reposo;
		f_callback = callback; 
		for (uint32_t i = 0; i < BUTTONS_NUMBER; ++i) 			{
			hal_gpio_sentido(button_list[i], HAL_GPIO_PIN_DIR_INPUT);
			hal_habilitar_int(button_list[i]);
		}

		// no se si estos dos suscribir tendrían que ir dentro de callback de butoton_list
		svc_GE_suscribir(ev_BOTON_RETARD0, drv_botones_tratar);
		svc_GE_suscribir(ev_PULSAR_BOTON, drv_botones_tratar);
	#endif //BUTTONS_NUMBER > 0	

}

// En auxiliar me llega el pin que ha sido activado 
void drv_botones_tratar(EVENTO_T evento, uint32_t auxiliar){
	switch (estado_actual){
	case e_reposo:
		if (evento == ev_PULSAR_BOTON) {
			
			// Hay que transformar este pin a botones ?? o se usa directamente el pin?
			
			
			// Encolar boton pulsado con el id_botón correcto
			//f_callback_drv(ev_PULSAR_BOTON, boton_equivalente_pin(auxiliar));
			
			

			// Activar una alarma esporádica (Trp) para 5 ms
			svc_alarma_activar(svc_alarma_codificar(0, TRP), ev_BOTON_RETARDO, auxiliar);
			
			// Desabilitar interrupciones
			for(int i = 0; i < BUTTONS_NUMBER; i++){
				hal_deshabilitar_int(button_list[i]);
			}
			
			// Cambiar al estado 'e_entrando'
			estado_actual = e_entrando;
			
    }
		break;
	
	case e_entrando: 
		if (evento == ev_BOTON_RETARDO) {			
			// Programar una alarma periódica (Tep)
			svc_alarma_activar(svc_alarma_codificar(0, TEP), ev_BOTON_RETARDO, auxiliar);
			
			// Cambiar al estado 'e_esperando'
			estado_actual = e_esperando;
		}
		break;
	
	case e_esperando:
		// Manejar el evento ev_BOTON_RETARDO en el estado 'e_esperando'
		if (evento == ev_BOTON_RETARDO) {
			// Verificar si el botón se ha soltado
			if (!boton_pulsado(auxiliar)) {
				// Programar una alarma esporádica (Trd)
				svc_alarma_activar(svc_alarma_codificar(0, TRD), ev_BOTON_RETARDO, auxiliar);
				
				// Saltar al estado 'e_soltado'
				estado_actual = e_soltado;
			}
			// Si el botón sigue pulsado se sigue en el estado esperando
			else{
				svc_alarma_activar(svc_alarma_codificar(0, TEP), ev_BOTON_RETARDO, auxiliar);
				estado_actual = e_esperando;
			}
        }
		break;
	
	case e_soltado:
		if(evento == ev_BOTON_RETARDO){
			// Limpiar interrupciones pendientes
			hal_ext_int_limpiar(); 
			// Habilitar las interrupciones nuevamente
			for (uint32_t i = 0; i < BUTTONS_NUMBER; ++i) {
				hal_habilitar_int(button_list[i]);
			}
			// Saltar al estado 'e_reposo'
			estado_actual = e_reposo;
		}
		break;
	
	default:
		break;
	}
}

