#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>


int main(int argc, char *argv[]){
    char message[255];
    int server, portNumber;
    struct sockaddr_in servAdd;     // server socket address

    if(argc != 3){
        printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
        exit(0);
    }

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "Cannot create socket\n");
        exit(1);
    }

    servAdd.sin_family = AF_INET;
    sscanf(argv[2], "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber);

    if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0){
        fprintf(stderr, " inet_pton() has failed\n");
        exit(2);
    }


    if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
        fprintf(stderr, "connect() has failed, exiting\n");
        exit(3);
    }

    int serverScore=0;
    int clientScore=0;
    int dice;
    char clientRoll[1];
    srand(24);
    while(1){
        //client reads and displays message from the server
        if (read(server, message, 255)==0){
            fprintf(stdout, "server not available...exiting\n");
            exit(3);
        }
        printf("Server's message: %s", message);
        printf("press enter to continue");
        //wait for enter to continue
        while( getchar() != '\n' );
        //client rolls his dice
        dice =rand() % 10 +1;
        clientScore+=dice;
        sprintf(clientRoll,"%d\n",dice);
        printf("sending to server\n");
        //client sends his dice roll to server
        write(server, clientRoll, strlen(clientRoll));

        if (read(server, message, 255)==0){
            fprintf(stdout, "connection closed...exiting\n");
            exit(3);
        }
        //client calculates server score and prints appropriate messages
        printf("i rolled %d my current score is %d\n", dice, clientScore);
        int serverDice = atoi(message);
        serverScore+= serverDice;
        printf("server rolled %d server current score is %d\n", serverDice, serverScore);
    }
}
