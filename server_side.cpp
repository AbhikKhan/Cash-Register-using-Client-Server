#include <iostream>
#include <map>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <csignal>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

int ServerFD, clientSocket, PORT;
void ChildProcess(int, int);
void handler(int);

map<int, pair<string, int>> Products;

void getProducts(){
    FILE *fp = fopen("Product.txt", "r");
    char buff[256], pro[256];
    int uid, price;
    while(fgets(buff, 256, fp)){
        sscanf(buff, "%d %s %d", &uid, pro, &price);
        Products[uid] = {string(pro), price};
    }
}

int main(int argc, char *argv[]) {

    if(argc < 2){
        printf("Less arguments.\n");
        exit(0);
    }

    getProducts();

    PORT = atoi(argv[2]);// port number from commandline

    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Child process id
    pid_t childpid;

    // Creating socket file descriptor
    if ((ServerFD = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(ServerFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    // address.sin_addr.s_addr = inet_addr(argv[1]); // using input ip address
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); // using input port

    // Forcefully attaching socket to the port
    if (bind(ServerFD, (struct sockaddr *)&address,
                                 sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listining to the port
    if (listen(ServerFD, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    else printf("Listining...\n");

    // To handle server termination
    signal(SIGINT, handler);

    while(1){
        clientSocket = accept(ServerFD, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (clientSocket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        // creating child process
        if((childpid = fork()) == 0){
            close(ServerFD); // closing child's listining socket as it is serving a request 
            ChildProcess(childpid, clientSocket);
            close(clientSocket); //child closes its version of connsd after computation is done (return from childprocess())
			exit(0); //child terminates
        }
        close(clientSocket);
    }
    return 0;
}

void ChildProcess(int pID, int clientSocket){
    int total = 0; // keep tracks of total item value
    char buffer[2048] = {0};
    string response = "";
    
    while(1){
        memset(buffer, 0, sizeof(buffer));
        // reading request sent by client
        int valread = read(clientSocket, buffer, 2048);
        
        if(valread < 0){
            response = "3 Error Receiving Command.";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            close(clientSocket);
            break;
        }
        
        if(strcmp(buffer, "404") == 0){
            printf("Abruptly termination from client.\n");
            close(clientSocket);
            exit(0);
        }
        
        char* token = strtok(buffer, " ");
        int request_type = atoi(token);
        
        if(request_type == 0){
            int UPC, Q, t = 0;
            while (token != NULL) {
                token = strtok(NULL, " ");
                if(t == 0)UPC = atoi(token);
                if(t == 1)Q = atoi(token);
                t++;
            }
            
            if(t < 2){
                response = "2 Protocol_Error Packet Discarded.";
                send(clientSocket , response.c_str() , strlen(response.c_str()) , 0);
            }
            else{
                string productName = Products[UPC].first;

                if(productName == ""){// error
                    response = "1 UPC is not found in database";
                }
                else{
                    response = "0 "; // valid response
                    int p = Q*Products[UPC].second; // calculaitng total price
                    total += p; // updating total price

                    response += to_string(p) + " " + productName;
                }
                
                send(clientSocket , response.c_str() , strlen(response.c_str()) , 0);
                // cout<<response<<endl;
            }
        }
        else{
            response = "0 " + to_string(total);
            // sending total price to the client
            send(clientSocket , response.c_str() , strlen(response.c_str()) , 0);
            break;
        }
    }
}

void handler(int num){
    string response = "4 Server terminated!";
    printf("\nServer Terminated...\n");
	send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

	close(ServerFD);
    exit(num);
}
