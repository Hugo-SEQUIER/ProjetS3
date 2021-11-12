#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/uip.h"
#include "net/rpl/rpl.h"

#include "net/netstack.h"
#include "dev/button-sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dev/leds.h"


#define DEBUG DEBUG_PRINT

#include "net/uip-debug.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

#define UDP_CLIENT_PORT    8765
#define UDP_SERVER_PORT    5678

#define UDP_EXAMPLE_ID  190
static struct uip_udp_conn *server_conn;

PROCESS(udp_server_process,
"UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);

/*---------------------------------------------------------------------------*/
/** Ici on gère la reception du message **/
static void
tcpip_handler(void) {
    char *appdata;

    if (uip_newdata()) {
        /** On stock dans appdata le contenu du message **/
        appdata = (char *) uip_appdata;
        appdata[uip_datalen()] = '\0';
        /** On affiche le message reçu **/
        PRINTF("On m'a envoye '%s' de ", appdata);
        leds_on(LEDS_BLUE);
        /** On affiche l'expéditeur du message **/
        PRINTF("%d",
               UIP_IP_BUF->srcipaddr.u8[sizeof(UIP_IP_BUF->srcipaddr.u8) - 1]);
        PRINTF("\n");
        /** On envoie un accusé de reception **/
        uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
        uip_udp_packet_send(server_conn, "Message bien envoye !", sizeof("Message bien envoye !"));
        uip_create_unspecified(&server_conn->ripaddr);

    }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data
)
{
uip_ipaddr_t ipaddr;
struct uip_ds6_addr *root_if;

PROCESS_BEGIN();

PROCESS_PAUSE();

SENSORS_ACTIVATE(button_sensor);

/** Ici on set l'adresse qui est la même que dans sender.c**/
#if UIP_CONF_ROUTER
/* Le choix de l'adresse du serveur détermine sa compression d'en-tête 6LoWPAN.
 * Évidemment, le choix fait ici doit également être sélectionné dans udp-client.c.
 *
 * Pour un décodage Wireshark correct à l'aide d'un renifleur, ajoutez le préfixe /64 aux préférences du protocole 6LowPAN,
 * par exemple. définissez Contexte 0 sur aaaa ::. À l'heure actuelle, Wireshark copie Context/128, puis l'écrase.
 * (La définition du contexte 0 sur aaaa::1111:2222:3333:4444 signalera une adresse compressée 16 bits de aaaa::1111:22ff:fe33:xxxx)
 * Remarque La vérification de la somme de contrôle IPCMV6 de Wireshark dépend des bonnes adresses non compressées.
 */

#if 0
/* Mode 1 - 64 bits inline */
   uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
#elif 1
/* Mode 2 - 16 bits inline */
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0x00ff, 0xfe00, 1);
#else
/* Mode 3 - derived from link local (MAC) address */
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
#endif
/** Création du DooDag **/
  uip_ds6_addr_add(&ipaddr, 0, ADDR_MANUAL);
  root_if = uip_ds6_addr_lookup(&ipaddr);
  if(root_if != NULL) {
    rpl_dag_t *dag;
    dag = rpl_set_root(RPL_DEFAULT_INSTANCE,(uip_ip6addr_t *)&ipaddr);
    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    rpl_set_prefix(dag, &ipaddr, 64);
    PRINTF("created a new RPL dag\n");
  } else {
    PRINTF("failed to create a new RPL DAG\n");
  }
#endif /* UIP_CONF_ROUTER */
NETSTACK_MAC.off(1);

/** On set la connection **/
server_conn = udp_new(NULL, UIP_HTONS(UDP_CLIENT_PORT), NULL);
if(server_conn == NULL) {
PRINTF("No UDP connection available, exiting the process!\n");

PROCESS_EXIT();

}
/** On lie la connection au local port **/
udp_bind(server_conn, UIP_HTONS(
UDP_SERVER_PORT));

while(1) {
/** Le process est en attente **/
PROCESS_YIELD();
/** Si le process reçoit un event tcp **/
if(ev == tcpip_event) {
tcpip_handler();

/** Si on clique sur le bouton du mote **/
} else if (ev == sensors_event && data == &button_sensor) {
PRINTF("Initiaing global repair\n");
/** On refait le DoDaG**/
rpl_repair_root(RPL_DEFAULT_INSTANCE);
}
}

PROCESS_END();

}
/*---------------------------------------------------------------------------*/
