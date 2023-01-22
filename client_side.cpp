#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char hello[] = "Hello from client";

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    else{
        int response_type;
        std::string msg;
        while(1){
            char buffer[2048] = {0};
            std::cout<<"0 to get item, 1 to close the connection\n";
            std::cin>>response_type;
            if(response_type == 0){
                // Send a message to the server
                int upc, quantity;
                std::cout<<"Enter the item UPC Code and quantity\n";
                std::cin>>upc>>quantity;
                
                msg = std::to_string(response_type) + " " + std::to_string(upc) + " " + std::to_string(quantity);

                send(sock , msg.c_str() , strlen(msg.c_str()) , 0);
                printf("Request sent\n");

                // Read the server's response
                valread = recv( sock , buffer, 2048, 0);
                printf("%s\n",buffer );
            }
            else{
                send(sock , "1" , 1 , 0);
                printf("Closing request sent\n");
                valread = recv(sock, buffer, 2048, 0);
                printf("%s\n",buffer );
                break;
            }
        }
    }
    return 0;
}
