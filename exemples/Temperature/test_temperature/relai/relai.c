#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/uip-udp-packet.h"
#include "sys/ctimer.h"


#ifdef WITH_COMPOWER
#include "powertrace.h"
#endif

#include <stdio.h>
#include <string.h>

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

PROCESS(udp_client_process,
"UDP sender process");
AUTOSTART_PROCESSES(&udp_client_process);


/** Ici on set l'adresse qui est la même que dans receiver.c**/
static void set_global_address(void) {
    uip_ipaddr_t ipaddr;

    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

/* Le choix de l'adresse du serveur détermine sa compression d'en-tête 6LoWPAN.
 * (Notre adresse sera compressée en Mode 3 puisqu'elle est dérivée de notre adresse lien-local)
 * Évidemment, le choix fait ici doit également être sélectionné dans udp-server.c.
 *
 * Pour un décodage Wireshark correct à l'aide d'un renifleur, ajoutez le préfixe /64 aux préférences du protocole 6LowPAN,
 * par exemple. définissez Contexte 0 sur aaaa ::. À l'heure actuelle, Wireshark copie Context/128, puis l'écrase.
 * (La définition du contexte 0 sur aaaa::1111:2222:3333:4444 signalera une adresse compressée 16 bits de aaaa::1111:22ff:fe33:xxxx)
 *
 * Notez que la vérification de la somme de contrôle IPCMV6 dépend des bonnes adresses non compressées.
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

PROCESS_THREAD(udp_client_process, ev, data
){
static struct etimer periodic;
static struct ctimer backoff_timer;


PROCESS_BEGIN();

PROCESS_PAUSE();

set_global_address();

/** On set la connection **/
client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL);
if(client_conn == NULL) {
PRINTF("Erreur on stop tout!\n");

PROCESS_EXIT();

}
/** On lie la connection au local port **/
udp_bind(client_conn, UIP_HTONS(
UDP_CLIENT_PORT));

etimer_set(&periodic, SEND_INTERVAL);
while(1) {
PROCESS_YIELD();

if(etimer_expired(&periodic)) {
etimer_reset(&periodic);
}
}

PROCESS_END();

}
