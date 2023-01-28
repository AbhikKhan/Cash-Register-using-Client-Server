#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

int sock = 0, valread, PORT;
    
void handler(int);

int main(int argc, char *argv[]) {

    if(argc < 2){
        printf("Less arguments.\n");
        exit(0);
    }
    PORT = atoi(argv[2]); // getting port number from the command line
    struct sockaddr_in serv_addr;

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) { // using server address from the command line
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Abruptly closing
    signal(SIGINT, handler);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    else{
        int response_type;
        string msg = "";
        while(1){
            char buffer[2048] = {0};
            cout<<"0 to get item, 1 to close the connection\n";
            cin>>response_type;
            if(response_type == 0){
                // Send a message to the server
                string upc, quantity;
                cout<<"Enter the item UPC Code and quantity\n";
                cin>>upc>>quantity;
                
                msg = to_string(response_type) + " " + upc + " " + quantity;

                send(sock , msg.c_str() , strlen(msg.c_str()) , 0);

                // Read the server's response
                valread = recv(sock , buffer, 2048, 0);
                
                char* token = strtok(buffer, " ");
                int res_type = atoi(token), t = 0;

                if(res_type == 0){ // valid response
                    while (token != NULL) {
                        token = strtok(NULL, " ");
                        if(t == 0)printf("%s ", token);
                        if(t == 1)printf("%s\n", token);
                        t++;
                    }
                }
                else if(res_type == 1){ // product code not found
                    printf("UPC not found in database.\n");
                }
                else if(res_type == 2){ // Protocol error
                    printf("Packet Discarded.\n");
                }
                else if(res_type == 3 || res_type == 4){ // server terminated or error in sending
                    
                    token = strtok(NULL, " ");
                    printf("%s ", token);
                    token = strtok(NULL, " ");
                    printf("%s\n", token);

                    printf("Fatal Error...Closing...\n");
                    break;
                }
            }
            else{
                send(sock , "1" , 1 , 0);
                printf("Closing request sent\n");
                valread = recv(sock, buffer, 2048, 0);
                printf("%s\n", buffer);
                break;
            }
        }
    }
    close(sock);
    return 0;
}

void handler(int num){
    string response = "404";
    printf("\nClient Terminated...\n");
	send(sock, response.c_str(), strlen(response.c_str()), 0);

	close(sock);
    exit(num);
}
