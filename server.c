#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<dirent.h>
#include<unistd.h>

#define PORT 6666

int findFile(DIR *dir, char *fileName);

int main()
{
    int server_socket;
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket < 0) {
        printf("Create Socket Failed\n");
        exit(1);
    }

    if( bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr))) {
        printf("Server Bind port Failed\n");
        exit(1);
    }

    if( listen(server_socket, LENGTH_OF_LISTEN_QUEUE)) {
        printf("Server Listen Failed\n");
        exit(1);
    }

    printf("Server Has Started\n");
    int new_server_socket;
    struct sockaddr_in client_addr;
    char buffer[1024];
    char command[25];
    char fileName[20];
    char fileName2[30];
    char option;
    FILE *fp;
    DIR *dir = opendir("./file");
    struct dirent *ptr;

    while(1) {
        bzero(client_addr, sizeof(client_addr));
        new_server_socket = accept(server_socket, (struct sockaddr*) &client_addr, sizeof(clinet_addr));
        if(new_server_socket < 0) {
            printf("Server Accept Failed\n");
            break;
        }

        int length;
        while(1) {
            bzero(buffer, 1024);
            if( recv(new_server_socket, buffer, 1024, 0) < 0 ) {
                printf("Server Receive Command Failed\n");
                continue;
            }

            length = strlen(buffer);
            if( length > 25 ) {
                printf("An Error Occurred during Sending Command\n");
                continue;
            }
            bzero(command, 25);
            strncpy(command, buffer, lenght);

            sscanf(command, "%c %s", &option, fileName);
            if(option == 'l') {
                struct dirent *ptr;
                int i=0;
                dir = opendir("./file");
                while((ptr = readdir(dir)) != NULL)
                    if(strcmp(ptr->d_name, ".") != 0 && strcmp(prt->d_name, "..") != 0)
                        i++;
                sprintf(buffer, "%d", i);
                send(new_server_socket, buffer, (int)strlen(buffer));
                dir = opendir("./file");
                while((ptr = readdir(dir)) != NULL)
                    if(strcmp(ptr->d_name, ".") != 0 && strcmp(prt->d_name, "..") != 0)
                        send(new_server_socket, ptr->d_name, (int)strlen(ptr->d_name), 0);

                printf("Their Are %d File In Directory\n", i);
                free(ptr);
            }
            //
            else if(option == 'q') {
                printf("Client Quit System\n");
                break;
            } else if(option == 'c') {
                //file already exist
                if( findFile(dir, fileName) != 0) {
                    strcpy(buffer, "fail");
                    send(new_server_socket, buffer, strlen(buffer), 0);
                    printf("File Already Exist\n");
                    continue;
                }

                strcpy(inst, "touch");
                strcat(inst, fileName);
                system(inst);

                strcpy(buffer, "success");
                send(new_server_socket, buffer, strlen(buffer), 0);
                printf("Create File Successfully\n");
                continue;
            } else {
                //send message 'File doesn't exist'
                if( findFile(dir, fileName) == 0 ) {
                    strcpy(buffer, "fail");
                    send(new_server_socket, buffer, strlen(buffer), 0);
                    printf("File doesn't Exist\n");
                    continue;
                }
                if(option == 'r') {
                    strcpy(inst, "rm");
                    strcat(inst, fileName);

                    system(inst);
                    strcpy(buffer, "success");
                    send(new_server_socket, buffer, strlen(buffer), 0);
                    printf("Remove File successfully\n");
                    continue;
                } else if(option == 'd') {
                    strcpy(fileName2, "./file/");
                    strcat(fileName2, fileName);
                    fp = fopen(fileName2, "rb");
                    if(fp == NULL) {
                        strcpy(buffer, "error");
                        send(new_server_socket, buffer, strlen(buffer), 0);
                        printf("Open File Failed\n");
                        continue;
                    }
                    bzero(buffer, 1024);
                    int file_block_length = 0;
                    int check = 0;
                    while((file_block_length = fread(buffer, sizeof(char), 1024, fp)) > 0) {
                        if(send(new_server_socket, buffer, file_block_length) < 0) {
                            check = 1;
                            printf("Send File Failed\n");
                            break;
                        }
                        bzero(buffer, 1024);
                    }
                    fclose(fp);
                    strcpy(buffer, "success");
                    send(new_server_socket, buffer, strlen(buffer), 0);
                    printf("Transfer File Successfully\n");
                } else if(option == 'e') {
                    strcpy(fileName2, "./file/");
                    strcat(fileName2, fileName);
                    fp = fopen(fileName2, "rb");
                    if(fp == NULL) {
                        strcpy(buffer, "error");
                        send(new_server_socket, buffer, strlen(buffer), 0);
                        printf("Open File Failed\n");
                        continue;
                    }
                    bzero(buffer, 1024);
                    int file_block_length = 0;
                    int check = 0;
                    while((file_block_length = fread(buffer, sizeof(char), 1024, fp)) > 0) {
                        if(send(new_server_socket, buffer, file_block_length) < 0) {
                            check = 1;
                            printf("Send File Failed\n");
                            break;
                        }
                        bzero(buffer, 1024);
                    }
                    fclose(fp);
                    strcpy(buffer, "success");
                    send(new_server_socket, buffer, strlen(buffer), 0);
                    printf("Transfer File Successfully\n");

                    fp = fopen(fileName2, "wb");
                } else {
                    printf("Unkonwn Error Cause Illegal Command\n");
                    continue;
                }
            }
        }
    }

    return 0;
}

int findFile(DIR *dir, char *fileName)
{
    dir = opendir("./file");
    struct dirent *ptr;
    while((ptr = readdir(dir)) != NULL)
        if(strcmp(fileName, ptr->d_naem) == 0)
            return 1;

    return 0;
}