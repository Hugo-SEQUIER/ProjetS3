#include "C:\Users\loicr\OneDrive - umontpellier.fr\Documents\Mes cours\Semestre 3\PROJET\contiki\core\contiki.h"
#include "3_exemple_processus.c"

static char msg[] = "Donnée";

static void example_function(void)
{
    process_start(&exemple_process,NULL); //démarre le processus

    //ici on envoi tous les types d'événement à notre processus
    process_post_synch(&exemple_process, PROCESS_EVENT_CONTINUE, msg); //envoi un événement au processus
    //de manière synchrone
    process_post(&exemple_process, PROCESS_EVENT_CONTINUE,msg); //envoi un événement au processus
    //de manière asynchrone
    process_poll(&exemple_process); //envoi un vote au processus
}