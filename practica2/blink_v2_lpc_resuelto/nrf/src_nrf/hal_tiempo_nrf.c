/* *****************************************************************************
 * P.H.2024: hal_tiempo_nrf.c
 * implementacion para cumplir el hal_tiempo.h
 * Nekane Diaz Montoya   870795	
 * Jorge Hernandez Aznar 872838
 */
 
#ifndef HAL_TIEMPO
#define HAL_TIEMPO
 
#include <nrf.h>

#include <stdint.h>

#define MAX_COUNTER_VALUE 0xFFFFFFFE			// Maximo valor del contador de 32 bits
#define HAL_TICKS2US	  16 					// frecuencia en MHz a la que funciona nrf52840 según si preserve es 0  (16 MHz / 2^Preserve)

/* *****************************************************************************
 * Timer0 contador de ticks
 */
static volatile uint32_t timer0_int_count;	// contador de 32 bits de veces que ha saltado la RSI Timer0

/* *****************************************************************************
 * Timer 0 Interrupt Service Routine
 */
void TIMER0_IRQHandler(void) {
    if (NRF_TIMER0->EVENTS_COMPARE[0]) {    
        NRF_TIMER0->EVENTS_COMPARE[0] = 0;  // Limpiar la interrupción
        timer0_int_count++;                 // Cada vez que llega el evento se aumenta el contador para poder almacenar los ticks
    }
}

/* *****************************************************************************
 * Programa un contador de tick sobre Timer0, con maxima precisión y minimas interrupciones
 */
uint32_t hal_tiempo_iniciar_tick() {
    timer0_int_count = 0;                       // Inicialmente no ha recibido interrupciones

    NRF_TIMER0->TASKS_STOP = 1;                 // Detener el temporizador
    NRF_TIMER0->MODE = TIMER_MODE_MODE_Timer;   // Modo temporizador 
    NRF_TIMER0->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos; // 32 bits para máxima precisión
    
    // ftimer = 16 MHz / (2^prescaler) 
    NRF_TIMER0->PRESCALER = 0;                  // Divisor de frecuencia a 1 MHz (16 MHz / 2^0 = 16 MHz)

    NRF_TIMER0->CC[0] = MAX_COUNTER_VALUE;      // Configurar el valor máximo de comparación
   
    // IRQ's habilitadas para COMPARE0
	NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;  

    // Restablece el timer cuando salta COMPARE0
	NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
    
    NVIC_EnableIRQ(TIMER0_IRQn);     // Habilitar la interrupción en el NVIC

    NRF_TIMER0->TASKS_CLEAR = 1;     // Limpiar el contador
    NRF_TIMER0->TASKS_START = 1;     // Iniciar el temporizador

    return HAL_TICKS2US;   // Devuelve el factor de conversión de ticks a microsegundos
}

/**
 * Lee el tiempo que lleva contando el contador y lo devuelve en ticks.
 */
uint64_t hal_tiempo_actual_tick() { 
	uint64_t time;
    NRF_TIMER0->TASKS_CAPTURE[0] = 1;  // Captura el valor actual del contador en CC[0]
    // Multiplica por el valor máximo del contador por las veces que ha llegado al tope más el ticks actuales
    time = ((MAX_COUNTER_VALUE + 1) * timer0_int_count) + (uint32_t)NRF_TIMER0->CC[0]; 
	return time;
}

/* *****************************************************************************
 * Activacion periodica con timer 1
 */
static void(*f_callback)();		//puntero a funcion a llamar cuando salte la RSI (en modo irq)

/* *****************************************************************************
 * Timer 1 Interrupt Service Routine
 * llama a la funcion de callbak que se ejecutara en modo irq
 */
void TIMER1_IRQHandler(void) {
    if (NRF_TIMER1->EVENTS_COMPARE[0]) {
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;  // Limpiar la interrupción
        if (f_callback) {
            f_callback();   // Llamar a la función de callback
        }
    }
}

/* *****************************************************************************
 * Dependiente del hardware Timer1
 * Programa el reloj para que llame a la funci�n de callback cada periodo.
 * El periodo se indica en tick. Si el periodo es cero se para el temporizador. 
 */
void hal_tiempo_reloj_periodico_tick(uint32_t periodo_en_tick, void (*funcion_callback)(void)) {
    
    f_callback = funcion_callback;

    if (periodo_en_tick != 0) {
        NRF_TIMER1->TASKS_STOP = 1;                 // Detener el temporizador
        NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;   // Modo temporizador
        NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos; // 32 bits
        NRF_TIMER1->PRESCALER = 0;                  // Prescaler para 16 MHz 

        NRF_TIMER1->CC[0] = periodo_en_tick - 1;    // Configurar el periodo

        NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos; // Habilitar interrupción

        // Restablece el timer cuando salta COMPARE0
	    NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;

        NVIC_EnableIRQ(TIMER1_IRQn);                // Habilitar la interrupción en el NVIC

        NRF_TIMER1->TASKS_CLEAR = 1;                // Limpiar el contador
        NRF_TIMER1->TASKS_START = 1;                // Iniciar el temporizador
    } else {
        // Detener el temporizador
        NRF_TIMER1->TASKS_STOP = 1;
        NVIC_DisableIRQ(TIMER1_IRQn);               // Deshabilitar la interrupción
    }
}

#endif
