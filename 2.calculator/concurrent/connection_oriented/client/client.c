#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 14000

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &(serverAddr.sin_addr)) <= 0) {
        perror("Invalid server IP address");
        exit(1);
    }

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Failed to connect to server");
        exit(1);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, SERVER_PORT);

    // Read user input and send requests
    while (1) {
        char request[256];
        char response[256];

        // Get the user input
        printf("Enter a calculator request (e.g 5 + 3): ");
        fgets(request, sizeof(request), stdin);

        // Remove newline character from user input
        size_t len = strlen(request);
        if (len > 0 && request[len - 1] == '\n')
            request[len - 1] = '\0';

        // Send request to server
        ssize_t bytesWritten = write(clientSocket, request, strlen(request));
        if (bytesWritten < 0) {
            perror("Failed to send request");
            exit(1);
        }

        // Receive and print server response
        ssize_t bytesRead = read(clientSocket, response, sizeof(response) - 1);
        if (bytesRead <= 0) {
            printf("Server disconnected\n");
            break;
        }

        response[bytesRead] = '\0';
        printf("Response: %s\n", response);
    }

    // Close client socket
    close(clientSocket);

    return 0;
}
