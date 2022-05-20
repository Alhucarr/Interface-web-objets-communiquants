/* A simple server in the internet domain using TCP The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

#include "common.h"

void error(const char *msg)
{
        perror(msg);
        exit(1);
}

#define users_register_path "./register.txt"
#define max_buffer 100

//  return  0 if already exist
//          1 if not

//  client register structure (x = 0 or 1)
//  client_name : x      
int client_register(char* client, int name_size, int vote)
{
    int fd = open(users_register_path, O_CREAT | O_RDWR);
    if(fd < 0)
    {
        printf("Server : error open users register\n");
        exit(1);
    }
    char r_buffer[max_buffer];
    char c;
    int index = 0;
    while(read(fd, &c, 1) != 0)
    {
        if(c != ' ')
        {
            r_buffer[index] = c;
            index++;
        }
        else
        {
            r_buffer[index] = 0;
            if(strcmp(r_buffer, client) == 0)
            {
                close(fd);
                return 0;
            }
            else
            {
                lseek(fd, 5, SEEK_CUR);
                index = 0;
            }
        }
    }
    char buffer[max_buffer+4];

    strcpy(buffer, client);
    strncat(buffer, " : ", 3);
    strncat(buffer, (char*)&vote, 1);
    strncat(buffer, "\n\0", 2);

    if(write(fd, buffer, name_size+5) < 0)
    {
        printf("Server : error write on users register\n");
        exit(1);
    }
    close(fd);
    return 1;
}

//PROTOCOLE
// client send
// name_size -> name -> vote(reponse sur 1 bit, 0 : été - 1 : hiver)
void protocole(int dsClient)
{
    unsigned int nbOctetRecu = 0;
    unsigned int nbCallRecv = 0;
    unsigned int nbOctetSend = 0;
    unsigned int nbCallSend = 0;

    int status = 0;

    int name_size = 0;
    status = recvTCP(dsClient, (char*)&name_size, sizeof(name_size), &nbOctetRecu, &nbCallRecv);
    if(status < 1)
    {
        printf("Serveur : pb recv_1 (name_size)\n");
        close(dsClient);
        exit(1);
    }

    char* client_name = malloc(name_size);
    status = recvTCP(dsClient, client_name, name_size, &nbOctetRecu, &nbCallRecv);
    if(status < 1)
    {
        printf("Serveur : pb recv_2 (client_name)\n");
        close(dsClient);
        exit(1);
    }

    int vote = 0;
    status = recvTCP(dsClient, (char*)&vote, 1, &nbOctetRecu, &nbCallRecv);
    if(status < 1)
    {
        close(dsClient);
        exit(1);
    }

    char buffer[50];
    if(client_register(client_name, name_size, vote) == 1)
    {
        strcpy(buffer, "a voté\0");
    }
    else
    {
        printf("already client\n");
        strcpy(buffer, "erreur, vote déjà réalisé\0");
    }

    int resp_size = strlen(buffer);
    status = sendTCP(dsClient, (char*)&resp_size, sizeof(resp_size), &nbOctetSend, &nbCallSend);
    if(status < 1)
    {
      printf("Serveur: pb sendTCP_1 (size reponse du serveur)\n");
      close(dsClient);
      exit(1);
    }

    status = sendTCP(dsClient, buffer, resp_size, &nbOctetSend, &nbCallSend);
    if(status < 1)
    {
      printf("Serveur: pb sendTCP_2 (reponse du serveur)\n");
      close(dsClient);
      exit(1);
    }

    free(client_name);
    close(dsClient);
}

int main(int argc, char *argv[])
{
        int sockfd, newsockfd, portno;
        socklen_t clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        int n;

        if (argc < 2) {
                fprintf(stderr, "ERROR, no port provided\n");
                exit(1);
        }

        // 1) on crée la socket, SOCK_STREAM signifie TCP

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                error("ERROR opening socket");

        // 2) on réclame au noyau l'utilisation du port passé en paramètre 
        // INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                error("ERROR on binding");


        // On commence à écouter sur la socket. Le 5 est le nombre max
        // de connexions pendantes

        listen(sockfd, 5);
        printf("Listening\n");
        while (1) {
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0)
                    error("ERROR on accept");

                printf("New client : %d\n", newsockfd);
                protocole(newsockfd);
        }

        close(sockfd);
        return 0;
}