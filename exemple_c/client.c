PROCESS_THREAD(udp_client_process, ev, data) {
    static char str[32];
    uip_ipaddr_t dest_ipaddr;
    PROCESS_BEGIN();

    /* Initialisation du socket UDP */
    simple_udp_register(&udp_client_socket, UDP_CLIENT_PORT,
                        NULL, UDP_SERVER_PORT, NULL);

    while(1) {     
        // Il faut toujours vérifier si le le mote root est joignable
        if(NETSTACK_ROUTING.node_is_reachable()
            && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {

            snprintf(str, sizeof(str), "Ping du client", count);
            // Envoyer le message à l'IP du mote root
            // (Cette adresse peut être aussi l'IP d'un autre mote)
            simple_udp_sendto(&udp_client_socket, str, strlen(str),
                              &dest_ipaddr);
        } else {
            // Communication impossible si le mote root est déconnecté
            LOG_INFO("Not reachable yet\n");
        }
    }
    PROCESS_END();
}