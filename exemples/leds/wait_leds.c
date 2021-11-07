#include "contiki.h"
#include <stdio.h> /* pour printf() */
#include "leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(ledsAlter, "Allumer 3 leds");
AUTOSTART_PROCESSES(&ledsAlter);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ledsAlter, ev, data)
{

  static struct etimer et; // Struct utilisée pour timer
  static int nbtour;

  PROCESS_BEGIN();

	nbtour=0;
	while(1){
		

		leds_off(LEDS_BLUE);//eteind la led bleu (utile dans ce while)
		leds_on(LEDS_RED);//allume la led rouge

		etimer_set(&et, 1 * CLOCK_SECOND);//remonte le timer
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));//écoule le timer

		leds_off(LEDS_RED);
		leds_toggle(LEDS_GREEN);//allume la led verte car eteinte de base

		etimer_set(&et, 1 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		leds_toggle(LEDS_GREEN);//eteind la led verte car allumée juste avant
		leds_on(LEDS_BLUE);


		etimer_set(&et, 1 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		nbtour+=1;
		printf("Tour: %d\n",nbtour);//amusement supp pour compter le nombre de tour d'allumage de leds.

	}
	 
  PROCESS_END();
}