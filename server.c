#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

void serviceClient(int);

int main(int argc, char *argv[]){
    int sd, client, portNumber;
    //struct for port and addr
    struct sockaddr_in servAdd;     // server socket address

    if(argc != 2){
        printf("Call model: %s <Port Number>\n", argv[0]);
        exit(0);
    }

    //AF_INET is for internet while af_unix is for same machine......sock_stream is for tcp udp is sock_dgram...0 means auto select protocol
    sd = socket(AF_INET, SOCK_STREAM, 0);
    //accept any ip addr
    servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber);

    bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
    listen(sd, 5);

    while(1){
        client = accept(sd, NULL, NULL);
        printf("Got a client\n");

        //fork is done and child proccess runs the game
        if(!fork())
            serviceClient(client);

        close(client);
    }
}

void serviceClient(int sd){
    char message[255];

    int clientPoints = 0;
    int serverPoints = 0;
    int dice;
    char serverRoll[3];
    dup2(sd, STDOUT_FILENO);
    srand(time(0));
    while(1){
        //server checks if game is over or not and sends appropriate message
        if(clientPoints>=100) {
            write(STDOUT_FILENO, "Game Over You Won\n", 19);
            close(sd);
            exit(0);
        }
        if(serverPoints>=100) {
            write(STDOUT_FILENO, "Game Over You Lost\n", 20);
            close(sd);
            exit(0);
        }
        write(STDOUT_FILENO, "Game On You Play\n", 18);
        //server reads the client roll from client
        if(!read(sd, message, 255)){
            close(sd);
            fprintf(stderr, "Bye, my client is dead, wait for a new one\n");
            exit(0);
        }
        //server adds to client points
        int clientDice = atoi(message);
        clientPoints+=clientDice;

        //server rolls own dice
        dice =rand() % 10 + 1;
        serverPoints+= dice;

        sprintf(serverRoll,"%d\n",dice);
        
        write(STDOUT_FILENO, serverRoll, strlen(serverRoll));
        usleep(250*1000);
    }
}