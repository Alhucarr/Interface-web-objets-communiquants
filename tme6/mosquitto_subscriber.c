// mosquitto_subscriber_IOCprojet.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <mosquitto.h>

#define HASH_SIZE 100

double start_t_glob;

//structure d'une case dans la table de hash
struct Entry {
	int time;
	char* data;
	int data_int;
	struct Entry* next;
};

struct HashMap {
	struct Entry* table[HASH_SIZE];
};

struct HashMap table;

//libère la table
void free_table() {
	struct Entry *parcours, *parcours2;
	int i = 0;
	
	for(i = 0; i < HASH_SIZE; i++) {
		parcours = table.table[i];
		
		while(parcours) {
			parcours2 = parcours->next;
			free(parcours->data);
			free(parcours);
			
			parcours = parcours2;
		}
	}
}

//ajoute l'entrée à la table
void add_to_table(struct Entry *new_entry) {
	int index_table = new_entry->time % HASH_SIZE;

	struct Entry *parcours = table.table[index_table], *parcours2 = parcours;

	if (!parcours) {
		table.table[index_table] = new_entry;
		return;
	}

	while (parcours) {
		parcours2 = parcours;
		parcours = parcours->next;
	}

	parcours2->next =new_entry;
	parcours = new_entry;
}

//affiche la table
void affiche_table() {

	size_t i;
	struct Entry *parcours;
	printf("\nBase de données (table de hachage) :\n");

	for (i = 0; i < HASH_SIZE; i++)
	{
		parcours = table.table[i];
		printf("%d. ", i);

		while(parcours) {
			printf("data : %s temps : %d\t", (parcours->data ), parcours->time);

			parcours = parcours->next;
		}
		printf("\n");
	}
}

//renvoie une moyenne de la luminosité de la minute demandée
int trouve_valeur() {	
	struct Entry *parcours;
	int i, j = 0, moyenne = 0;
	
	for(i=0; i < HASH_SIZE; i++) {
		parcours = table.table[i];
		while(parcours) { //on parcours la liste chainée
			moyenne += parcours->data_int;
			j++;
			
			parcours = parcours->next;
		}
	}
	
	return moyenne/j;
}

//fonction appelée lorsque l'on se connecte au broker
void on_connect(struct mosquitto* mosq, void* obj, int rc) {
	printf("ID: %d\n", *(int*)obj);
	if (rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
	
	//on subscribe au topic de l'esp32 et celui du site pour recevoir des requêtes
	mosquitto_subscribe(mosq, NULL, "esp32/luminosity", 0);
	mosquitto_subscribe(mosq, NULL, "database/input", 0);
}

//fonction appelée lorsqu'on reçoit un message sur n'importe quel topic
void on_message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg) {
	printf("New message with topic %s\n", msg->topic);
	
	if(!strcmp(msg->topic, "esp32/luminosity")) {
		//on viens de recevoir un message de l'esp32 avec des informations sur la luminosité
		char* new_data = malloc(256);
		strcpy(new_data, msg->payload);
		new_data[strlen(new_data)] = 0;
		
		//timestamp en minutes
		double time_stamp = (((double)(clock())) - start_t_glob);	
		time_stamp = time_stamp;
		printf("timestamp %lf\n", time_stamp);

		// on crée une nouvelle entrée qu'on ajoute ensuite à la table de hash
		struct Entry *new_entry = malloc(sizeof(struct Entry));
		new_entry->time = (int)time_stamp;
		new_entry->data = new_data;
		new_entry->data_int = atoi(msg->payload);
		
		new_entry->next = NULL;
		add_to_table(new_entry);
		affiche_table();
		return;
	}
	else {
		//on viens de recevoir une requête du site, on lui retourne la moyenne de toute les
		//luminosités reçu de l'esp32
		printf("Calcul luminosité moyenne.\n");
		int moyenne = trouve_valeur();
		
		//conversion en char*
		char buffer[10];
		sprintf(buffer, "%d", moyenne);
		printf("Luminosite moyenne = %s\n", buffer);
		
		//on publish un message contenant la moyenne vers le site
		if(mosquitto_publish(mosq, NULL, "database/output", strlen(buffer), buffer ,0, false) == MOSQ_ERR_SUCCESS) {
				printf("Moyenne sended success.\n");
		}
		else {
			printf("Failed sending moyenne.\n");
		}
	}
}

int main()
{
	int rc, id = 12; 
	
	start_t_glob = (double)clock();

    mosquitto_lib_init();

    struct mosquitto* mosq;
	
	//instanciation
	mosq = mosquitto_new("subscribe-test", true, &id);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);

	rc = mosquitto_connect(mosq, "localhost", 1883, 10);
	if (rc) {
		printf("Could not connect to Broker with return code %d\n", rc);
		return -1;
	}
	
	//on boucle en attendant les messages mqtt auquels on a subscribe
	mosquitto_loop_start(mosq);
	printf("Press Enter to quit...\n");
	getchar();
	mosquitto_loop_stop(mosq, true);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	
	//libère la table de hash
	free_table();

	return 0;
}
