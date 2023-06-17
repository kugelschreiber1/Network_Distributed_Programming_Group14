/*
CONNECTION LESS CLIENT
Socket type: SOCK_DGRAM
Accept connections: recvfrom()
Send responses: sendto()
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 15000

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddr.sin_addr)) <= 0) {
        perror("Invalid server IP address");
        exit(1);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    // Read user input and send requests
    while (1) {
        char request[256];
        char response[256];

        printf("Enter a calculator request (e.g 5 + 3): ");
        fgets(request, sizeof(request), stdin);

        // Remove newline character from user input
        size_t len = strlen(request);
        if (len > 0 && request[len - 1] == '\n')
            request[len - 1] = '\0';

        // Send request to server
        ssize_t bytesSent = sendto(clientSocket, request, strlen(request), 0,
                                   (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        if (bytesSent < 0) {
            perror("Failed to send request");
            exit(1);
        }

        // Receive and print server response
        struct sockaddr_in serverResponseAddr;
        socklen_t serverResponseAddrLen = sizeof(serverResponseAddr);
        ssize_t bytesRead = recvfrom(clientSocket, response, sizeof(response) - 1, 0,
                                     (struct sockaddr *)&serverResponseAddr, &serverResponseAddrLen);
        if (bytesRead <= 0) {
            printf("Failed to receive response\n");
            break;
        }

        response[bytesRead] = '\0';
        printf("Response from %s:%d: %s\n",
               inet_ntoa(serverResponseAddr.sin_addr), ntohs(serverResponseAddr.sin_port), response);
    }

    // Close client socket
    close(clientSocket);

    return 0;
}
