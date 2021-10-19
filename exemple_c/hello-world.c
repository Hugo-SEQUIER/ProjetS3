#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/light-sensor.h"
#include "dev/leds.h"
#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data) {
	PROCESS_BEGIN();
	/* Déclaration des variables */
	int static compteur = 0;
	int static blinks = 0;
	static struct etimer et;
	etimer_set(&et, CLOCK_SECOND*60);
	SENSORS_ACTIVATE(button_sensor);
	SENSORS_ACTIVATE(light_sensor);
	/* Algo */
	while(1) {
		PROCESS_WAIT_EVENT();
			/* Test si on clique sur le bouton */
			if (ev == sensors_event && data == &button_sensor){
				compteur = compteur+1;
				if (compteur % 2 == 0) {
					leds_off(LEDS_BLUE);
					leds_off(LEDS_RED);
					leds_on(LEDS_RED);
				}
				else {
					leds_off(LEDS_BLUE);
					leds_off(LEDS_RED);
					leds_on(LEDS_BLUE);
				}
				printf("Light: \%u\n", light_sensor.value(0));
				char* x = "Hello";
				printf("\%s\n", x);
				printf("\%i\n", compteur);
			}
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
