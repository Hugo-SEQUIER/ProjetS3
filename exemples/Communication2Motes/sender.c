#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/uip-udp-packet.h"
#include "sys/ctimer.h"
#include "dev/button-sensor.h"
#ifdef WITH_COMPOWER
#include "powertrace.h"
#endif

#include <stdio.h>
#include <string.h>
#include "dev/leds.h"

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define UDP_EXAMPLE_ID  190

#define DEBUG DEBUG_PRINT

#include "net/uip-debug.h"

#ifndef PERIOD
#define PERIOD 60
#endif

#define START_INTERVAL        (15 * CLOCK_SECOND)
#define SEND_INTERVAL        (PERIOD * CLOCK_SECOND)
#define SEND_TIME        (random_rand() % (SEND_INTERVAL))
#define MAX_PAYLOAD_LEN        30


static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process,
"UDP client process");
AUTOSTART_PROCESSES(&udp_client_process);

/*---------------------------------------------------------------------------*/
static void tcpip_handler(void) {
    char *str;

    if (uip_newdata()) {
        str = uip_appdata;
        str[uip_datalen()] = '\0';
        printf("J'ai reçu : '%s'\n", str);
    }
}

/*---------------------------------------------------------------------------*/
static void send_packet(void *ptr) {
    char buf[MAX_PAYLOAD_LEN];

    PRINTF("J'envoie à  %d 'Salut'\n", server_ipaddr.u8[sizeof(server_ipaddr.u8) - 1]);
    leds_on(LEDS_RED);
    sprintf(buf, "<3", 1);
    uip_udp_packet_sendto(client_conn, buf, strlen(buf), &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
}

/*---------------------------------------------------------------------------*/
static void set_global_address(void) {
    uip_ipaddr_t ipaddr;

    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

/* The choice of server address determines its 6LoPAN header compression.
 * (Our address will be compressed Mode 3 since it is derived from our link-local address)
 * Obviously the choice made here must also be selected in udp-server.c.
 *
 * For correct Wireshark decoding using a sniffer, add the /64 prefix to the 6LowPAN protocol preferences,
 * e.g. set Context 0 to aaaa::.  At present Wireshark copies Context/128 and then overwrites it.
 * (Setting Context 0 to aaaa::1111:2222:3333:4444 will report a 16 bit compressed address of aaaa::1111:22ff:fe33:xxxx)
 *
 * Note the IPCMV6 checksum verification depends on the correct uncompressed addresses.
 */

#if 0
    /* Mode 1 - 64 bits inline */
       uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
#elif 1
/* Mode 2 - 16 bits inline */
    uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0x00ff, 0xfe00, 1);
#else
    /* Mode 3 - derived from server link-local (MAC) address */
      uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0x0250, 0xc2ff, 0xfea8, 0xcd1a); //redbee-econotag
#endif
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data
)
{
static struct etimer periodic;
static struct ctimer backoff_timer;
#if WITH_COMPOWER
static int print = 0;
#endif

PROCESS_BEGIN();

PROCESS_PAUSE();
SENSORS_ACTIVATE(button_sensor);
set_global_address();

/* new connection with remote host */
client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL);
if(client_conn == NULL) {
PRINTF("Erreur on stop tout!\n");

PROCESS_EXIT();

}
udp_bind(client_conn, UIP_HTONS(
UDP_CLIENT_PORT));

PRINTF("Created a connection with the server ");
PRINT6ADDR(&client_conn->ripaddr);
PRINTF(" local/remote port %u/%u\n",
UIP_HTONS(client_conn
->lport),
UIP_HTONS(client_conn
->rport));

#if WITH_COMPOWER
powertrace_sniff(POWERTRACE_ON);
#endif

etimer_set(&periodic, SEND_INTERVAL);
while(1) {
PROCESS_YIELD();

if(ev == tcpip_event) {
tcpip_handler();

}

if(etimer_expired(&periodic) && data== &button_sensor) {
etimer_reset(&periodic);
ctimer_set(&backoff_timer, SEND_TIME, send_packet, NULL);

#if WITH_COMPOWER
if (print == 0) {
powertrace_print("#P");
}
if (++print == 3) {
print = 0;
}
#endif

}
}

PROCESS_END();

}
/*---------------------------------------------------------------------------*/
