#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 20000

int main()
{
    int clientSocket, ret;
    struct sockaddr_in cliAddr;
    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Client Socket is created.\n");

    memset(&cliAddr, '\0', sizeof(cliAddr));
    memset(buffer, '\0', sizeof(buffer));

    cliAddr.sin_family = AF_INET;
    cliAddr.sin_port = htons(PORT);
    cliAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = connect(clientSocket, (struct sockaddr*)&cliAddr, sizeof(cliAddr));
    if (ret < 0) {
        printf("Error in connection.\n");
        exit(1);
    }

    printf("Connected to Server.\n");

    char serialNumber[20];
    char regNumber[20];
    char name[100];

    printf("Enter Serial Number: ");
    scanf("%s", serialNumber);
    printf("Enter Registration Number: ");
    scanf("%s", regNumber);
    printf("Enter Name: ");
    scanf(" %[^\n]", name);

    // Concatenate the registration details into a single string
    snprintf(buffer, sizeof(buffer), "Serial Number: %s, Registration Number: %s, Name: %s", serialNumber, regNumber, name);

    send(clientSocket, buffer, strlen(buffer), 0);

    while (1) {
        if (recv(clientSocket, buffer, 1024, 0) < 0) {
            printf("Error in receiving data.\n");
        } else {
            printf("Server: %s\n", buffer);
            break;  // Exit the loop after receiving the server's response
        }
    }

    close(clientSocket);
    return 0;
}