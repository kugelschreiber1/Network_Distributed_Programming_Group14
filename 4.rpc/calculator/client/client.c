#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "clientStub.h"


int main() {
    // Send request message
    int operand1, operand2;
    char operator;
    char request[100];

    printf("Enter first operand: ");
    scanf("%d", &operand1);

    printf("Enter operator (-, +, /, *): ");
    scanf(" %c", &operator);

    printf("Enter second operand: ");
    scanf("%d", &operand2);

    // Prepare the request
    sprintf(request, "%d %c %d", operand1, operator, operand2);

    //Call the client stub
    clientStub(request);
    return 0;
}
