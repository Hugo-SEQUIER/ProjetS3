#include "contiki.h"
#include <stdio.h> /* pour printf() */
#include "leds.h"
#include "dev/button-sensor.h"
/*---------------------------------------------------------------------------*/
PROCESS(ledsAlter, "Allumer button leds");
AUTOSTART_PROCESSES(&ledsAlter);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ledsAlter, ev, data)
{
  static int nbtour;
  PROCESS_BEGIN();
	
	nbtour=0;
	SENSORS_ACTIVATE(button_sensor);//active les sensors

	while(1){
		
		PROCESS_WAIT_EVENT_UNTIL(ev==sensors_event && data==&button_sensor);//Attendre jusqu'à que je clique

		leds_off(LEDS_BLUE);//eteind la led bleu (utile dans ce while)
		leds_on(LEDS_RED);//allume la led rouge

		PROCESS_WAIT_EVENT_UNTIL(ev==sensors_event && data==&button_sensor);
		
		leds_off(LEDS_RED);
		leds_toggle(LEDS_GREEN);//allume la led verte car eteinte de base

		PROCESS_WAIT_EVENT_UNTIL(ev==sensors_event && data==&button_sensor);

		leds_toggle(LEDS_GREEN);//eteind la led verte car allumée juste avant
		leds_on(LEDS_BLUE);
		
		nbtour+=1;
		printf("Tour: %d\n",nbtour);//amusement supp pour compter le nombre de tour d'allumage de leds.

	} 
  PROCESS_END();
}