#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char message[1024] = {0};
    int num1, num2;
    char op;

    // Create a socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Get user input for the two numbers and operation
    printf("Enter two integers and chose the operations to perform\n\n");
    printf("Enter first integer: ");
    scanf("%d", &num1);

    printf("Enter second integer: ");
    scanf("%d", &num2);

    printf("Enter operation (+, -, *, /): ");
    scanf(" %c", &op);

    // Prepare the message to send to the server
    sprintf(message, "%d %c %d", num1, op, num2);
    printf("Sending message: %s\n", message);

    // Send the message to the server
    send(sock, message, strlen(message), 0);

    // Wait for the response from the server
    valread = read(sock, buffer, 1024);

    // Print the result
    printf("%s\n", buffer);

    return 0;
}

