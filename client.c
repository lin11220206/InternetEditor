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

    if(connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0 ) {
        printf("Connect to Server Failed\n");
        exit(1);
    }

    char command[50];
    char inst[10];
    char fileName[20];
    char buffer[1024];
    FILE *fp;
    printf("Welcom to Internet Editor\n");
    printf("You can enter 'help' to see help info \n");
    while(1) {
        bzero(command, 50);
        bzero(fileName, 20);
        bzero(buffer, 1024);

        fgets(command, sizeof(command), stdin);

        //print help info
        if( strcmp(command, "help") == 0 ) {
            printf("To Create File: create [fileName]\n");
            printf("To Edit File: create [fileName]\n");
            printf("To Remove File: remove [fileName]\n");
            printf("To List File: list\n");
            printf("To Downlaod File: download [fileName]\n");
            printf("To Quit System: quit\n");
            continue;
        }

        sscanf(command, "%s %s", inst, fileName);

        if (strcmp(inst, "create") == 0) {
            //send instruction
            bzero(buffer, 1024);
            strcpy(buffer, "c ");
            strcat(buffer, fileName);
            send(client_socket, buffer, strlen(buffer), 0);

            //receive message
            bzero(buffer, 1024);
            if( recv(client_socket, buffer, 1024, 0) < 0)
                printf("Receive Message Failed\n");

            if(strcmp(buffer, "fail")
                    printf("%s Already Exist\n", fileName);
                    else if(strcmp(buffer, "success")
                                printf("Create %s Successfully\n", fileName);
                                else
                                    printf("Unkown Error\n");

                } else if(strcmp(inst, "remove") == 0) {
            bzero(buffer, 1024);
            strcpy(buffer, "r ");
            strcat(buffer, fileName);
            send(client_socket, buffer, strlen(buffer), 0);

            bzero(buffer, 1024);
            if( recv(client_socket, buffer, 1024, 0) < 0)
                printf("Receive Message Failed\n");

            if(strcmp(buffer, "fail")
                    printf("%s Doesn't Exist\n", fileName);
                    else if(strcmp(buffer, "success")
                                printf("Remove %s Successfully\n", fileName);
                                else
                                    printf("Unkown Error\n");

                } else if(strcmp(inst, "list") == 0) {
            bzero(buffer, 1024);
            strcpy(buffer, "l");
            send(client_socket, buffer, strlen(buffer), 0);

            bzero(buffer, 1024);
            if( recv(client_socket, buffer ,1024, 0) <0)
                printf("Receive Message Failed\n");

            printf("List Of Directory: \n");
            int i, number = atoi(buffer);
            for(i=0; i<number; ++i) {
                bzero(buffer, 1024);
                recv(client_socket, buffer, 1024, 0);
                printf("\t%s\n", buffer);
            }

        } else if(strcmp(inst, "download") == 0) {
            bzero(buffer, 1024);
            strcpy(buffer, "d ");
            strcat(buffer, fileName);
            send(client_socket, buffer, strlen(buffer), 0);

            bzero(buffer, 1024);
            if( recv(client_socket, buffer ,1024, 0) <0)
                printf("Receive Message Failed\n");

            if(strcmp(buffer, "fail") == 0)
                printf("%s Doesn't Exist\n", fileName);
            else if(strcmp(buffer, "error") == 0)
                printf("Open %s Failed\n", fileName);
            else {
                fp = fopen(fileName, "wb");
                if(fp == NULL) {
                    printf("Can't Open %s To Write\n", fileName);
                    continue;
                }

                bzero(buffer, 1024);
                int length = 0;
                while ( length = recv(client_socket, buffer, 1024, 0)) {
                    if(length < 0) {
                        printf("Receive Data From Server Failed\n");
                        break;
                    }

                    int write_length = fwrite(buffer, sizeof(char), length, fp);
                    if(write_length < length) {
                        printf("Write File Failed\n");
                        break;
                    }
                    bzero(buffer, 1024);
                }
            }
        } else if(strcmp(inst, "edit") == 0) {
        } else if(strcmp(inst, "quit") == 0) {
            bzero(buffer, 1024);
            strcpy(buffer, "q");
            send(client_socket, buffer, strlen(buffer), 0);

            close(client_socket);
            return 0;

        } else {
            printf("Illegal Command, you can enter 'help' to see help info\n");
            continue;
        }

    }

    return 0;
}
