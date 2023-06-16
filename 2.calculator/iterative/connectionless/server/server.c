/*
CONNECTION LESS SERVER
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
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 13000

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
            if (operand2 == 0)
            {
                result = 0; // Division by zero
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

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
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

    printf("Calculator server is running and listening on port %d\n", PORT);

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        char request[256];
        char response[256];
        int operand1, operand2;
        char operator;

        // Receive request from client
        ssize_t bytesRead = recvfrom(serverSocket, request, sizeof(request) - 1, 0,
                                     (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (bytesRead <= 0) {
            perror("Failed to receive request");
            continue;
        }

        request[bytesRead] = '\0';
        printf("Received request from %s:%d: %s\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), request);

        // Parse request
        sscanf(request, "%d %c %d", &operand1, &operator, &operand2);

        // Process request
        int result = calculate(operand1, operand2, operator);

        // Formulate response
        snprintf(response, sizeof(response), "Result: %d\n", result);

        // Send reply back to client
        ssize_t bytesWritten = sendto(serverSocket, response, strlen(response), 0,
                                      (struct sockaddr *)&clientAddr, clientAddrLen);
        if (bytesWritten < 0) {
            perror("Failed to send reply");
            continue;
        }

        printf("Sent response to %s:%d: %s\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), response);
    }

    // Close server socket
    close(serverSocket);

    return 0;
}
