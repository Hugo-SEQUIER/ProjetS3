#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(light_leds, "Allumer 3 leds");
AUTOSTART_PROCESSES(&light_leds);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(light_leds, ev, data)
{
  PROCESS_BEGIN();

  	leds_on(LEDS_ALL);
  
  PROCESS_END();
}