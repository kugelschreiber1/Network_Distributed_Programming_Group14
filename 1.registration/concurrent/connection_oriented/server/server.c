#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 20000
#define FILENAME "students.txt"

struct Student {
    char serialNumber[20];
    char regNumber[20];
    char name[100];
};

int validateRegistrationFormat(const struct Student* student) {
    // Check if all fields have valid values
    if (strlen(student->serialNumber) == 0 ||
        strlen(student->regNumber) == 0 ||
        strlen(student->name) == 0) {
        return 0;  // Invalid format
    }

    return 1;  // Valid format
}

int isSerialNumberDuplicate(const struct Student* student) {
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        return 0;  // File doesn't exist, so no duplicates
    }

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        struct Student existingStudent;
        sscanf(line, "%s\t%s\t%[^\n]",existingStudent.serialNumber, existingStudent.regNumber, existingStudent.name);

        if (strcmp(existingStudent.serialNumber, student->serialNumber) == 0) {
            fclose(file);
            return 1;  // Duplicate found
        }
    }

    fclose(file);
    return 0;  // No duplicates found
}

int isRegistrationNumberDuplicate(const struct Student* student) {
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        return 0;  // File doesn't exist, so no duplicates
    }

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        struct Student existingStudent;
        sscanf(line, "%s\t%s\t%[^\n]",existingStudent.serialNumber, existingStudent.regNumber, existingStudent.name);

        if (strcmp(existingStudent.regNumber, student->regNumber) == 0) {
            fclose(file);
            return 1;  // Duplicate found
        }
    }

    fclose(file);
    return 0;  // No duplicates found
}

int main() {
    int sockfd, ret;
    struct sockaddr_in serverAddr;
    int clientSocket;
    struct sockaddr_in cliAddr;
    socklen_t addr_size;
    pid_t childpid;
    FILE* file;

    if (access(FILENAME, F_OK) == 0) {
        printf("Registration file exists");
    } else {
        file = fopen(FILENAME, "w");
        if (file == NULL) {
            perror("Error in opening file");
            exit(1);
        }

        fprintf(file, "SerialNo\tRegistrationNo\tName\n");
        fclose(file);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error in socket creation");
        exit(1);
    }

    printf("Server Socket is created.\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        perror("Error in binding");
        exit(1);
    }

    if (listen(sockfd, 10) == 0) {
        printf("Listening...\n\n");
    }

    int client = 1;
    while (1) {
        clientSocket = accept(sockfd, (struct sockaddr*)&cliAddr, &addr_size);
        if (clientSocket < 0) {
            exit(1);
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
        printf("Clients %d connected:\n\n", client++);

        if ((childpid = fork()) == 0) {
            close(sockfd);

            char buffer[1024];
            memset(buffer, '\0', sizeof(buffer));

            // Receive registration details from client
            if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
                printf("Error in receiving data.\n");
                exit(1);
            }

            // Parse the received registration details
            struct  Student student;
            sscanf(buffer, "Serial Number: %[^,], Registration Number: %[^,], Name: %[^\n]",
                   student.serialNumber, student.regNumber, student.name);

            // Validate the registration format
            if (!validateRegistrationFormat(&student)) {
                send(clientSocket, "Invalid registration format.", strlen("Invalid registration format."), 0);
                close(clientSocket);
                exit(1);
            }

            // Check for duplicates
            if (isSerialNumberDuplicate(&student)) {
                send(clientSocket, "Duplicate serial number found.", strlen("Duplicate serial number found."), 0);
                close(clientSocket);
                exit(1);
            }

            if (isRegistrationNumberDuplicate(&student)) {
                send(clientSocket, "Duplicate registration number found.", strlen("Duplicate registration number found."), 0);
                close(clientSocket);
                exit(1);
            }

            // Open the file in append mode
            file = fopen(FILENAME, "a");
            if (file == NULL) {
                perror("Error in opening file");
                exit(1);
            }

            // Write registration details to the file
            fprintf(file, "%s\t%s\t%s\n",student.serialNumber, student.regNumber, student.name);

            // Close the file
            fclose(file);

            // Send a confirmation message to the client
            send(clientSocket, "Registration details saved", strlen("Registration details saved."), 0);

            close(clientSocket);
            exit(0);
        }

        close(clientSocket);
    }

    close(sockfd);
    return 0;
}