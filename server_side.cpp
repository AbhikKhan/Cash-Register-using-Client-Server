#include <iostream>
#include <vector>
#include <map>
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

using namespace std;
int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
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
        cout<<"--------Waiting for new connection-----------\n";
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // printing client details
        printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        cout<<"Clients connected: "<< ++cnt<<endl;
        // reading request
        if((childpid = fork()) == 0){
            int total = 0;
            char buffer[2048] = {0};
            while(1){
                valread = read(new_socket, buffer, 2048);
                cout<<"---------Received Request----------"<<endl;
                printf("%s\n",buffer);

                if(buffer[0] == '0'){
                    /*Need to implement function to calculate product value*/
                    // break buffer into string of tokens dilimiter " "
                    char* token = strtok(buffer, " ");
                    vector<string> req;
                    while (token != NULL) {
                        req.push_back(string(token));
                        token = strtok(NULL, " ");
                    }
                    cout<<"Requested Product is "<<req[1]<<endl;
                    cout<<"Requested Product quantity is "<<stoi(req[2])<<endl;
                    total++;
                    send(new_socket , hello , strlen(hello) , 0);
                    cout<<"Product Price sent"<<endl;
                }
                else{
                    string price = to_string(total);
                    send(new_socket , price.c_str() , strlen(price.c_str()) , 0);
                    cout<<"Total Price sent"<<endl;
                    close(new_socket);
                    --cnt;
                    break;
                }
            }
        }
    }
    return 0;
}