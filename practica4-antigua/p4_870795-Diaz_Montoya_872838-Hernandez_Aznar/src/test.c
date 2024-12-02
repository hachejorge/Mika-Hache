#include "hal_gpio.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"
#include "board.h"
#include "rt_GE.h"
#include "drv_botones.h"
#include "srv_alarm.h"
#include "rt_FIFO.h"
#include "hal_WDT.h"
#include "test.h"

void test_WDT(){
		hal_WDT_iniciar(10);
		drv_led_encender(1);
		while(1);				// se queda en un bucle infinito y debería salir de este al reiniciarse a los 10 segundos
}

void test_rt_FIFO(){
	rt_FIFO_inicializar(1);
	//verificar_insertar_extraer_en_cola();
	verificar_overflow_cola(); // inserta más eventos que tamaño de la cola para que se produzca un overflow
}

void leds_c(EVENTO_T evento, uint32_t id){
	drv_led_conmutar(id);			 // conmuta los leds
}

void test_alarmas(){
	svc_alarma_iniciar(1,rt_FIFO_encolar,ev_T_PERIODICO); // inciamos la alarma periódica del ev_T_PERIODICO
	svc_alarma_activar(svc_alarma_codificar(1,1000), ev_PULSAR_BOTON, 10); // activamos la alarma de pulsar boton cada segundo
	EVENTO_T evento;
  uint32_t auxiliar;
	Tiempo_us_t timestamp;
	while(true){
			if(rt_FIFO_extraer(&evento,&auxiliar,&timestamp)){ // extraemos de la fifo
					if(evento == ev_PULSAR_BOTON){		// si el evento es PULSAR_BOTON, conmutamos los leds
							leds_c(evento,1);
					}
			}
	}
}

void pruebas_botones(){
	drv_consumo_iniciar(3,4);	// incializamos drv_consumo_iniciar
	rt_FIFO_inicializar(2);   // incializamos la fifo
	rt_GE_iniciar(1);					// incializamos el gestor de eventos
	svc_alarma_iniciar(1,rt_FIFO_encolar,ev_T_PERIODICO); // inciamos la alarma periódica del ev_T_PERIODICO
	drv_botones_iniciar(rt_FIFO_encolar,ev_PULSAR_BOTON, ev_BOTON_RETARDO); // inicializamos los botones para que se encole cada vez que se produzca el ev_PULSAR BOTON y ev_BOTON_RETARDO
	svc_GE_suscribir(ev_PULSAR_BOTON,leds_c); // suscribimos el ev_PULSAR_BOTON a leds conmutar
	rt_GE_lanzador();					// incializamos el lanzador
}
