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
#include "drv_SC.h"
#include "rt_GE.h"
#include "srv_alarm.h"
#include <stddef.h>
#include <stdbool.h>

#define TRP 20 // Tiempo de rebote de entrada
#define TEP 5 // Periodo para comprobar si se ha soltado el botón
#define TRD 20 // Tiempo de rebote de salida

// Si está a 1 indica que el boton está pulsado, si está a 0 está sin pulsar
static bool estado_boton[BUTTONS_NUMBER];
static bool estaReseteando;

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

// Devuelve true si el pin seleccionado está activo, false en caso contrario
bool boton_pulsado(uint32_t pin) {
    return hal_ext_int_leer(pin) == BUTTONS_ACTIVE_STATE;
}


void actualizar_estado_botones(uint32_t boton){
	
	for(int i = 0; i < BUTTONS_NUMBER; i++){
			if( i+1 != boton){
					// si(! estaba pulsado && boton_pulsado) -> activar alarma
					if( !estado_boton[i] && boton_pulsado(button_list[i])){
						if(!estaReseteando){
								svc_alarma_activar(svc_alarma_codificar(0, 3000), ev_RESET_BOTON, 0);
								estaReseteando = true;
						}
					}
					// sino(estaba pulsado && !boton_pulsado) -> desactivar alarma
					else if(estado_boton[i] && !boton_pulsado(button_list[i])){
						svc_alarma_activar(svc_alarma_codificar(0, 0), ev_RESET_BOTON, 0);
						estaReseteando = false;
					}
			}
	}
	
}




static void(*f_callback)(EVENTO_T id, uint32_t aux);		//puntero a funcion a llamar cuando salte la RSI (en modo irq)

void f_callback_drv(uint32_t aux){
	// Desabilitar interrupciones
	for(int i = 0; i < BUTTONS_NUMBER; i++){
		hal_ext_int_deshabilitar(button_list[i]);
	}
	f_callback(ev_PULSAR_BOTON,boton_equivalente_pin(aux));
	
}

// En auxiliar me llega el pin que ha sido activado 
void drv_botones_tratar(EVENTO_T evento, uint32_t auxiliar){
	switch (estado_actual){
	case e_reposo:
		if (evento == ev_PULSAR_BOTON) {
			
			estaReseteando = false;

			// Activar una alarma esporádica (Trp) para 5 ms
			svc_alarma_activar(svc_alarma_codificar(0, TRP), ev_BOTON_RETARDO, auxiliar);
			
			// Cambiar al estado 'e_entrando'
			estado_actual = e_entrando;
        }
		break;
	
	case e_entrando: 
		if (evento == ev_BOTON_RETARDO) {			
			// Programar una alarma periódica (Tep)
			//svc_alarma_activar(svc_alarma_codificar(true, 0), ev_BOTON_RETARDO, auxiliar);
			svc_alarma_activar(svc_alarma_codificar(1, TEP), ev_BOTON_RETARDO, auxiliar);
			
			// Cambiar al estado 'e_esperando'
			estado_actual = e_esperando;
		}
		break;
	
	case e_esperando:
		// Manejar el evento ev_BOTON_RETARDO en el estado 'e_esperando'
		if (evento == ev_BOTON_RETARDO) {
			hal_ext_int_limpiar(); 
			
			actualizar_estado_botones(auxiliar);
			
			// Verificar si el botón se ha soltado
			if (!boton_pulsado(button_list[auxiliar-1])){
				// Programar una alarma esporádica (Trd)
				svc_alarma_activar(svc_alarma_codificar(0, TRD), ev_BOTON_RETARDO, auxiliar);
				
				// Saltar al estado 'e_soltado'
				estado_actual = e_soltado;
			}
			// Si el botón sigue pulsado se sigue en el estado esperando
			else{
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
				hal_ext_int_habilitar(button_list[i]);
			}
			// Saltar al estado 'e_reposo'
		
			if(estaReseteando){
					// Desactivamos una posible alarma activa de reseteo de botón
					svc_alarma_activar(svc_alarma_codificar(0,0), ev_RESET_BOTON, 0);
					estaReseteando = false;
			}
			
			estado_actual = e_reposo;
		}
		break;
	
	default:
		break;
	}
}


// OPERACIONES PÚBLICAS

// Inicia el módulo 
void drv_botones_iniciar(void(*callback)(EVENTO_T id, uint32_t aux), uint32_t _ev_pulsar_boton, uint32_t _ev_boton_retardo){
	#if BUTTONS_NUMBER > 0
		// Inicializar botones con el callback (rt_fifo_encolar) y otros parámetros
		hal_ext_int_iniciar(f_callback_drv);		
		estado_actual = e_reposo;
		f_callback = callback; 
		for (uint32_t i = 0; i < BUTTONS_NUMBER; ++i) 			{
			// Activamos el sentido para que para los pines de los botones se activen cada vez que se pulse un boton
			hal_gpio_sentido(button_list[i], HAL_GPIO_PIN_DIR_INPUT);
			// Activamos las interrupciones para los pines de los botones
			hal_ext_int_habilitar(button_list[i]);
			//
			estado_boton[i] = false;
			
		}
		estaReseteando = false;
	
		svc_GE_suscribir((EVENTO_T)_ev_pulsar_boton, drv_botones_tratar);
		svc_GE_suscribir((EVENTO_T)_ev_boton_retardo, drv_botones_tratar);
		
	#endif //BUTTONS_NUMBER > 0	

}

