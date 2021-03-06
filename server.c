#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<dirent.h>
#include<unistd.h>

int findFile(DIR *dir, char *fileName);

int main(int argc, char *argv[])
{
    if(argc != 2) {
        printf("Usage: %s [port]\n", argv[0]);
        exit(0);
    }
    //create server socket
    int server_socket;
    struct sockaddr_in server_addr;
    //bzero(&server_addr, sizeof(server_addr));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket < 0) {
        printf("Create Socket Failed\n");
        exit(1);
    }

    if( bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr))) {
        printf("Server Bind Port: %s Failed\n", argv[1]);
        exit(1);
    }
    //listen port
    if( listen(server_socket, 10)) {
        printf("Server Listen Failed\n");
        exit(1);
    }

    printf("Server Has Started\n");
    int new_server_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_length;
    char buffer[1024];
    char command[25];
    char fileName[20];
    char fileName2[30];
    char inst[40];
    char option;
    FILE *fp;
    DIR *dir = opendir("./file");
    struct dirent *ptr;

    while(1) {
        client_addr_length = sizeof(client_addr);
        memset(&client_addr, 0, client_addr_length);
        //bzero(client_addr, client_addr_length);

        //accept client socket
        new_server_socket = accept(server_socket, (struct sockaddr*) &client_addr, &client_addr_length);
        if(new_server_socket < 0) {
            printf("Server Accept Failed\n");
            break;
        }

        printf("Accept New Client\n");

        int length;
        while(1) {
            bzero(buffer, 1024);
            //receive command from client
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
            strncpy(command, buffer, length);

            sscanf(command, "%c %s", &option, fileName);
            //list
            if(option == 'l') {
                struct dirent *ptr;
                int i=0;
                dir = opendir("./file");
                while((ptr = readdir(dir)) != NULL)
                    if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
                        i++;
                sprintf(buffer, "%d", i);
                send(new_server_socket, buffer, 1024, 0);
                dir = opendir("./file");
                while((ptr = readdir(dir)) != NULL)
                    if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
                        send(new_server_socket, ptr->d_name, 1024, 0);

                printf("Their Are %d File In Directory\n", i);
                free(ptr);
            }
            //quit
            else if(option == 'q') {
                printf("Client Quit System\n");
                close(new_server_socket);
                break;
            }
            //create file
            else if(option == 'c') {
                //file already exist
                if( findFile(dir, fileName) != 0) {
                    strcpy(buffer, "fail");
                    send(new_server_socket, buffer, 1024, 0);
                    printf("File Already Exist\n");
                    continue;
                }

                strcpy(inst, "touch ./file/");
                strcat(inst, fileName);
                system(inst);

                strcpy(buffer, "success");
                send(new_server_socket, buffer, 1024, 0);
                printf("Create File Successfully\n");
            } else {
                //send message 'File doesn't exist'
                if( findFile(dir, fileName) == 0 ) {
                    strcpy(buffer, "fail");
                    send(new_server_socket, buffer, strlen(buffer), 0);
                    printf("File doesn't Exist\n");
                    continue;
                }
                //remove file
                if(option == 'r') {
                    strcpy(inst, "rm ./file/");
                    strcat(inst, fileName);

                    system(inst);
                    strcpy(buffer, "success");
                    send(new_server_socket, buffer, strlen(buffer), 0);
                    printf("Remove File successfully\n");
                }
                //download file
                else if(option == 'd') {
                    strcpy(fileName2, "./file/");
                    strcat(fileName2, fileName);
                    fp = fopen(fileName2, "rb");
                    if(fp == NULL) {
                        strcpy(buffer, "error");
                        send(new_server_socket, buffer, 1024, 0);
                        printf("Open File Failed\n");
                        continue;
                    }
                    int file_block_length = 0;
                    int file_block_number = 0;
                    //count block number
                    while(fread(buffer, sizeof(char), 1024, fp) > 0)
                        file_block_number++;

                    rewind(fp);
                    sprintf(buffer, "%d", file_block_number);
                    send(new_server_socket, buffer, 1024, 0);
                    bzero(buffer, 1024);
                    //send file
                    while((file_block_length = fread(buffer, sizeof(char), 1024, fp)) > 0) {
                        if(send(new_server_socket, buffer, file_block_length, 0) < 0) {
                            printf("Send File Failed\n");
                            break;
                        }
                        bzero(buffer, 1024);
                    }
                    fclose(fp);
                    printf("Transfer File Successfully\n");
                }
                //edit file
                else if(option == 'e') {

                    int check = 0;
                    strcpy(fileName2, "./file/");
                    strcat(fileName2, fileName);
                    fp = fopen(fileName2, "wb");
                    if(fp == NULL) {
                        printf("Access File Failed\n");
                        check = 1;
                    }

                    if( recv(new_server_socket, buffer, 1024, 0) < 0) {
                        printf("Receive Count Failed\n");
                        check = 1;
                    }
                    //get file_block_number
                    int count = atoi(buffer);
                    bzero(buffer, 1024);
                    int length = 0;
                    //save data
                    while(count--) {
                        length = recv(new_server_socket, buffer, 1024, 0);
                        if( length < 0) {
                            printf("Receive Data From Client Failed\n");
                            check = 1;
                            break;
                        }

                        int write_length = fwrite(buffer, sizeof(char), length, fp);
                        if(write_length < length) {
                            printf("Write File Failed\n");
                            check = 1;
                            break;
                        }
                        bzero(buffer, 1024);
                    }
                    fclose(fp);
                    if(check == 0)
                        strcpy(buffer, "success");
                    else
                        strcpy(buffer, "fail");

                    send(new_server_socket, buffer, 1024, 0);
                    bzero(buffer, 1024);
                    printf("End Edit File\n");
                } else {
                    printf("Unkonwn Error Cause Illegal Command\n");
                    continue;
                }
            }
        }
    }
    close(server_socket);
    return 0;
}
//check if file exist
int findFile(DIR *dir, char *fileName)
{
    dir = opendir("./file");
    struct dirent *ptr;
    while((ptr = readdir(dir)) != NULL)
        if(strcmp(fileName, ptr->d_name) == 0)
            return 1;

    return 0;
}
