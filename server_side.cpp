#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[2048] = {0};
    const char *hello = "Hello from server";

    // Child process id
    pid_t childpid;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int cnt = 0;
    while(1){
        std::cout<<"Waiting for new connection..\n";
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // printing client details
        printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        std::cout<<"Clients connected: "<< ++cnt<<std::endl;
        // reading request
        if((childpid = fork()) == 0){
            int total = 0;
            while(1){
                valread = read(new_socket, buffer, 2048);
                std::cout<<"---------Received Request----------"<<std::endl;
                printf("%s\n",buffer);

                if(buffer[0] == '0'){
                    /*Need to implement function to calculate product value*/
                    std::cout<<"Requested Product is "<<std::endl;
                    std::cout<<"Requested Product quantity is "<<std::endl;
                    send(new_socket , hello , strlen(hello) , 0);
                    std::cout<<"Product Price sent"<<std::endl;
                }
                else{
                    send(new_socket , hello , strlen(hello) , 0);
                    std::cout<<"Total Price sent"<<std::endl;
                    close(new_socket);
                    --cnt;
                    break;
                }
            }
        }
    }
    return 0;
}