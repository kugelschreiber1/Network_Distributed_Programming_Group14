/*
CONNECTION ORIENTED SERVER
Socket type: SOCK_STREAM
Accept connections: accept()
Send responses: send()
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 14000


// the calculator function
int calculate(int operand1, int operand2, char operator) {
    int result;
    switch (operator) {
        case '+':
            result = operand1 + operand2;
            break;
        case '-':
            result = operand1 - operand2;
            break;
        case '*':
            result = operand1 * operand2;
            break;
        case ' ':
            result = 0;
            break;        
        case '/':
            if (operand2 == 0) {
                result = 0;
                break;
            }
            result = operand1 / operand2;
            break;
        default:
            result = 0;  // Invalid operator
            break;
    }
    return result;
}

// Function for handling clients
void handleClient(int clientSocket, struct sockaddr_in clientAddr) {
    char request[256];
    char response[256];
    int operand1, operand2;
    char operator;

    // Read request
    ssize_t bytesRead = read(clientSocket, request, sizeof(request) - 1);
    if (bytesRead <= 0) {
        printf("Client disconnected\n");
        close(clientSocket);
        return;
    }

    request[bytesRead] = '\0';
    printf("Received request: %s\n", request);

    // Parse request
    sscanf(request, "%d %c %d", &operand1, &operator, &operand2);

    printf("Server is processing the following request %s",request);

    // Process request
    int result = calculate(operand1, operand2, operator);

    // Formulate response
    snprintf(response, sizeof(response), "Result: %d\n", result);

    // Send reply back to client
    ssize_t bytesWritten = write(clientSocket, response, strlen(response));
    printf("Client port: %d, %s",ntohs(clientAddr.sin_port), response);

    if (bytesWritten < 0) {
        perror("Failed to send reply");
    }

    // Close client connection
    close(clientSocket);
}



int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    // Bind socket to a well-known address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    // Place socket in passive mode(wait for client connections)
    if (listen(serverSocket, 1) < 0) {
        perror("Failed to listen for connections");
        exit(1);
    }

    printf("Calculator server is running and listening on port %d...\n", PORT);

    while (1) {
        // Accept next connection request and obtain a new socket
        clientAddrLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            perror("Failed to accept connection");
            exit(1);
        }

        printf("Client connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Create a new process (slave) to handle the request
        pid_t pid = fork();

        if (pid == -1) {
            perror("Failed to create a new process");
            exit(1);
        } else if (pid == 0) {
            // Child process (slave)
            close(serverSocket);  // Close the server socket in the slave process
            handleClient(clientSocket, clientAddr);
            exit(0);  // Terminate the slave process after handling the client
        } else {
            // Parent process (master)
            close(clientSocket);  // Close the client socket in the master process
        }
    }

    // Close server socket
    close(serverSocket);

    return 0;
}
