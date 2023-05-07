#include <stdio.h>             // Standard input/output functions
#include <stdlib.h>            // Standard library functions
#include <string.h>            // String manipulation functions
#include <sys/socket.h>        // Socket functions
#include <netinet/in.h>        // Internet address family functions
#include <netdb.h>             // Hostname lookup functions
#include <arpa/inet.h>         // Functions for manipulating IP addresses


#define BUFFER_SIZE 1024        // Maximum size of the buffer used for sending and receiving data

int main()
{    
    char url[1024];             
    printf("Enter the URL of the web page you want to retrieve: \n");
    fgets(url, 1024, stdin);

    // Remove the newline character from the end of the URL
    url[strcspn(url, "\n")] = '\0';

    // Parse the URL to get the host and path
    char *host = url + 7; // skip the "http://" prefix
    char *path = strchr(host, '/');
    if (path == NULL)
    {
        path = "/";
    }
    else
    {
        *path = '\0'; // terminate the host string
        path++;      // skip the '/'
    }

    // Lookup the IP address of the host
    struct hostent *he = gethostbyname(host);
    if (he == NULL)
    {
        printf("Could not resolve host name\n");
        return 1;
    }

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }

    // Connect to the server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect");
        close(sockfd);
        return 1;
    }

    // Send the HTTP request to the server
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
    if (send(sockfd, buffer, strlen(buffer), 0) == -1)
    {
        perror("send");
        close(sockfd);
        return 1;
    }

    // Receive the response from the server and print it to the console
    int bytes_received;
    do
    {
        bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1)
        {
            perror("recv");
            close(sockfd);
            return 1;
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    } while (bytes_received > 0);

    // Close the socket
    close(sockfd);

    return 0;
}
