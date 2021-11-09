#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "leds.h"
#include "wait.inc.c"
/*---------------------------------------------------------------------------*/
PROCESS(ledsAlter, "Allumer 3 leds");
AUTOSTART_PROCESSES(&ledsAlter);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ledsAlter, ev, data)
{

  PROCESS_BEGIN();

	nbtour=0;
	while(1){
		

		leds_off(LEDS_BLUE)//eteind la led bleu (utile dans ce while)
		leds_on(LEDS_RED);//allume la led rouge

		wait();

		leds_off(LEDS_RED);
		leds_toggle(LEDS_GREEN);//allume la led verte car eteinte de base

		wait();

		leds_toggle(LEDS_GREEN);//eteind la led verte car allumée juste avant
		leds_on(LEDS_BLUE);


		wait();

		nbtour+=1;
		printf("Tour%d\n",nbtour);//amusement supp pour compter le nombre de tour d'allumage de leds.

	}
	
		

  
  PROCESS_END();
}