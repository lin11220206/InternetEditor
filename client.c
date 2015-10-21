#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>

#define PORT 6666

int main()
{
    int client_socket;
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = PF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(client_socket < 0) {
        printf("Create Socket Failed\n");
        exit(1);
    }

    if( bind(clinet_socket, (struct sockaddr*) &client_addr, sizeof(client_addr))) {
        printf("Client Bind port Failed\n");
        exit(1);
    }

    struct sockaddr_in server_addr;
    bzero(&server_adder, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if(connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr) < 0 ) {
    printf("Connect to Server Failed\n");
        exit(1);
    }

    char command[50];
    char inst[10];
    char fileName[20];
    char buffer[1024];
    printf("Welcom to Internet Editor\n");
    printf("You can enter 'help' to see help info \n");
    while(1) {
    bzero(command, 50);
        bzero(fileName, 20);
        bzero(buffer, 1024);

        gets(command);

        //print help info
        if( strcmp(command, "help") == 0 ) {

            continue;
        }

        sscanf(command, "%s %s", inst, fileName);

        if ( strcmp(inst, "create") == 0) {

        } else if( strcmp(inst, "edit") == 0) {
        } else if( strcmp(inst, "remove") == 0) {
        } else if( strcmp(inst, "list") == 0) {
        } else if( strcmp(inst, "download") == 0) {
        } else if( strcmp(inst, "quit") == 0) {
        } else {
            printf("Illegal Command, you can enter 'help' to see help info\n");
            continue;
        }

    }

    return 0;
}