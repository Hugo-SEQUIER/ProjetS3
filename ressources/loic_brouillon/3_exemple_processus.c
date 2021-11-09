#include "~/mnt/c/Users/loicr/OneDrive - umontpellier.fr/Documents/Mes cours/Semestre 3/PROJET/contiki/core/contiki.h"

PROCESS (exemple_process, "Exemple de processus "); //définitions du bloc de contrôle du processus
AUTOSTART_PROCESSES(&exemple_process); //lorsque Contiki démarre, il doit automatiquement lancé le processus
PROCESS_THREAD(exemple_process,evenement,data) //début de la définitions du thread du processus
//evenement et data sont utilisées lorsque le processus reçoit des événements
{
	PROCESS_BEGIN(); //début du processus
	while(1) //on peut se permettre une boucle infini car nous attendons un événement plus bas
	{
		PROCESS_WAIT_EVENT(); //ici le noyau contrôle le processus et attend l'événement
		printf("Obtient le numéro d'événement %d \n", evenement); //imprime le numéro de l'événement
	}
	PROCESS_END(); //suppression des actifs, mais ici jamais atteinte, ce processus ne s'arrête pas seul
}

