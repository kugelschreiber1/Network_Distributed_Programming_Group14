#ifndef CLIENTSTUB_H_INCLUDED
#define CLIENTSTUB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void clientStub(char request[100]) {
    // Set up the socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Replace with your desired port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with server IP address

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Send the request
    send(sockfd, request, strlen(request), 0);

    // Receive response
    char response[100];
    recv(sockfd, response, sizeof(response), 0);

    // Process response
    printf("Result: %s\n", response);

    // Close connection
    close(sockfd);
}

#endif // CLIENTSTUB_H_INCLUDED