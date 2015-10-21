#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>

#define PORT 6666

int main(int argc, char *argv[])
{
    if(argc != 3) {
        printf("Usage: %s [ip] [port]\n", argv[0]);
        exit(1);
    }
    //Create client socket
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

    if( bind(client_socket, (struct sockaddr*) &client_addr, sizeof(client_addr))) {
        printf("Client Bind port Failed\n");
        exit(1);
    }

    //Store server information
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    if(inet_aton(argv[1], &server_addr.sin_addr) == 0) {
        printf("Server IP Address Error\n");
        exit(1);
    }
    server_addr.sin_port = htons(atoi(argv[2]));

    //Connect to server
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
        bzero(fileName, 25);
        bzero(buffer, 1024);

        printf(">");
        fgets(command, sizeof(command), stdin);
        sscanf(command, "%s %s", inst, fileName);

        //print help info
        if( strcmp(inst, "help") == 0 ) {
            printf("To Create File: create [fileName]\n");
            printf("To Edit File: edit [fileName]\n");
            printf("To Remove File: remove [fileName]\n");
            printf("To List File: list\n");
            printf("To Downlaod File: download [fileName]\n");
            printf("To Quit System: quit\n");
            continue;
        }

        else if (strcmp(inst, "create") == 0) {
            //send instruction
            bzero(buffer, 1024);
            strcpy(buffer, "c ");
            strcat(buffer, fileName);
            send(client_socket, buffer, 1024, 0);

            //receive message
            bzero(buffer, 1024);
            if( recv(client_socket, buffer, 1024, 0) < 0)
                printf("Receive Message Failed\n");

            if(strcmp(buffer, "fail") == 0)
                printf("%s Already Exist\n", fileName);
            else if(strcmp(buffer, "success") == 0)
                printf("Create %s Successfully\n", fileName);
            else
                printf("Unkown Error\n");

        } else if(strcmp(inst, "remove") == 0) {
            bzero(buffer, 1024);
            strcpy(buffer, "r ");
            strcat(buffer, fileName);
            send(client_socket, buffer, 1024, 0);

            bzero(buffer, 1024);
            if( recv(client_socket, buffer, 1024, 0) < 0)
                printf("Receive Message Failed\n");

            if(strcmp(buffer, "fail") == 0)
                printf("%s Doesn't Exist\n", fileName);
            else if(strcmp(buffer, "success") == 0)
                printf("Remove %s Successfully\n", fileName);
            else
                printf("Unkown Error\n");

        } else if(strcmp(inst, "list") == 0) {
            bzero(buffer, 1024);
            strcpy(buffer, "l");
            send(client_socket, buffer, 1024, 0);

            bzero(buffer, 1024);
            if( recv(client_socket, buffer ,1024, 0) <0)
                printf("Receive Message Failed\n");
            printf("File in Directory: \n");
            //get list length
            int i, number = atoi(buffer);
            //print list
            for(i=0; i<number; ++i) {
                bzero(buffer, 1024);
                recv(client_socket, buffer, 1024, 0);
                printf("\t%s\n", buffer);
            }
            printf("(end)\n");

        } else if(strcmp(inst, "download") == 0) {
            bzero(buffer, 1024);
            strcpy(buffer, "d ");
            strcat(buffer, fileName);
            send(client_socket, buffer, 1024, 0);

            bzero(buffer, 1024);
            if( recv(client_socket, buffer ,1024, 0) <0)
                printf("Receive Message Failed\n");

            if(strcmp(buffer, "fail") == 0)
                printf("%s Doesn't Exist\n", fileName);
            else if(strcmp(buffer, "error") == 0)
                printf("Access To %s Failed\n", fileName);
            else {
                fp = fopen(fileName, "wb");
                if(fp == NULL) {
                    printf("Can't Create %s In Client\n", fileName);
                    continue;
                }
                //get file_block_number
                int count = atoi(buffer);
                bzero(buffer, 1024);
                int length = 0;
                //save data
                while (count--) {
                    length = recv(client_socket, buffer, 1024, 0);
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
                fclose(fp);
                printf("Download %s Successfully\n", fileName);
            }
        } else if(strcmp(inst, "edit") == 0) {
            //download a temp file in this directory
            bzero(buffer, 1024);
            strcpy(buffer, "d ");
            strcat(buffer, fileName);
            send(client_socket, buffer, 1024, 0);

            bzero(buffer, 1024);
            if( recv(client_socket, buffer ,1024, 0) <0)
                printf("Receive Message Failed\n");

            if(strcmp(buffer, "fail") == 0)
                printf("%s Doesn't Exist\n", fileName);
            else if(strcmp(buffer, "error") == 0)
                printf("Access To %s Failed\n", fileName);
            else {
                char tmpName[25];
                strcpy(tmpName, fileName);
                strcat(tmpName, ".tmp");
                fp = fopen(tmpName, "wb");
                if(fp == NULL) {
                    printf("Can't Open %s\n", fileName);
                    continue;
                }
                int count = atoi(buffer);
                bzero(buffer, 1024);
                int length = 0;
                while (count--) {
                    length = recv(client_socket, buffer, 1024, 0);
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
                fclose(fp);

                //edit file with 'vi'
                char inst[30];
                bzero(inst, 30);
                strcpy(inst, "vi ");
                strcat(inst, tmpName);
                printf("System Will Use 'vi' To Edit File, Press Any Key To Continue\n");
                fgetc(stdin);
                system(inst);


                //send file back
                strcpy(buffer, "e ");
                strcat(buffer, fileName);
                send(client_socket, buffer, 1024, 0);
                bzero(buffer, 1024);

                fp = fopen(tmpName, "rb");
                if(fp == NULL) {
                    printf("Save File Failed\n");
                    continue;
                }
                int file_block_length = 0;
                int file_block_number = 0;
                while(fread(buffer, sizeof(char), 1024, fp) > 0)
                    file_block_number++;

                rewind(fp);
                sprintf(buffer, "%d", file_block_number);
                send(client_socket, buffer, 1024, 0);
                bzero(buffer, 1024);

                while((file_block_length = fread(buffer, sizeof(char), 1024, fp)) > 0) {
                    if(send(client_socket, buffer, file_block_length, 0) < 0) {
                        printf("Save File Failed\n");
                        break;
                    }
                    bzero(buffer, 1024);
                }
                fclose(fp);

                //remove temp file
                bzero(inst, 25);
                strcpy(inst, "rm ");
                strcat(inst, tmpName);
                system(inst);
                recv(client_socket, buffer, 1024, 0);
                if(strcmp(buffer,"success") == 0)
                    printf("Edit %s Successfully\n", fileName);
                else
                    printf("Edit %s Failed\n", fileName);
            }

        } else if(strcmp(inst, "quit") == 0) {
            bzero(buffer, 1024);
            strcpy(buffer, "q");
            //send message to let server know that this client quit
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
