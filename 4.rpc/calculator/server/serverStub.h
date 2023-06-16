#ifndef SERVERSTUB_H_INCLUDED
#define SERVERSTUB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "calculator.h"

void serverStub(int sockfd) {
    // Receive request from client
    char request[100];
    recv(sockfd, request, sizeof(request), 0);

    // Process request
    int operand1, operand2, result;
    char operator;
    sscanf(request, "%d %c %d", &operand1, &operator, &operand2);

    result = calculator(operand1, operand2, operator);

    // Formulate reply
    char reply[100];
    sprintf(reply, "%d", result);

    // Send reply message
    send(sockfd, reply, strlen(reply), 0);
}


#endif // SERVERSTUB_H_INCLUDED
