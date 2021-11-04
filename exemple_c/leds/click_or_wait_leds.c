#include "contiki.h"
#include <stdio.h> /* pour printf() */
#include "leds.h"
#include "dev/button-sensor.h"
/*---------------------------------------------------------------------------*/
PROCESS(ledsAlt, "Allumer button leds");
AUTOSTART_PROCESSES(&ledsAlt);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ledsAlt, ev, data)
{
  static int nbtour;
  static struct etimer et;
  
  PROCESS_BEGIN();

	nbtour=0;
	
	while(1){
		
		etimer_set(&et, 4 * CLOCK_SECOND);
	 	PROCESS_WAIT_EVENT_UNTIL((etimer_expired(&et)) || (ev==sensors_event && data==&button_sensor));


		leds_off(LEDS_BLUE);//eteind la led bleu (utile dans ce while)
		leds_on(LEDS_RED);//allume la led rouge

		etimer_set(&et, 4 * CLOCK_SECOND);
	 	PROCESS_WAIT_EVENT_UNTIL((etimer_expired(&et)) || (ev==sensors_event && data==&button_sensor));
		
		leds_off(LEDS_RED);
		leds_toggle(LEDS_GREEN);//allume la led verte car eteinte de base

		etimer_set(&et, 4 * CLOCK_SECOND);
	 	PROCESS_WAIT_EVENT_UNTIL((etimer_expired(&et)) || (ev==sensors_event && data==&button_sensor));

		leds_toggle(LEDS_GREEN);//eteind la led verte car allumée juste avant
		leds_on(LEDS_BLUE);
		
		nbtour+=1;
		printf("Tour: %d\n",nbtour);//amusement supp pour compter le nombre de tour d'allumage de leds.

	}
	
		

  
  PROCESS_END();
}
	 
	
