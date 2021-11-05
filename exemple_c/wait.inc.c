#include <stdio.h>
#include "contiki.h"

static struct etimer et;

int wait(){
	printf("%sok\n", );
    etimer_set(&et, 1 * CLOCK_SECOND);//remonte le timer
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));//écoule le timer
}