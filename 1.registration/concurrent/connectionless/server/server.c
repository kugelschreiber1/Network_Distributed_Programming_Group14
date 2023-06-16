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
        sscanf(line, "%s\t%s\t%[^\n]", existingStudent.serialNumber, existingStudent.regNumber, existingStudent.name);

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
        sscanf(line, "%s\t%s\t%[^\n]", existingStudent.serialNumber, existingStudent.regNumber, existingStudent.name);

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
    struct sockaddr_in cliAddr;
    socklen_t addr_size;
    pid_t childpid;
    FILE* file;

    if (access(FILENAME, F_OK) == 0) {
        printf("Registration file exists\n");
    } else {
        file = fopen(FILENAME, "w");
        if (file == NULL) {
            perror("Error in opening file");
            exit(1);
        }

        fprintf(file, "SerialNo\tRegistrationNo\tName\n");
        fclose(file);
    }

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  // Use SOCK_DGRAM for UDP
    if (sockfd < 0) {
        perror("Error in socket creation");
        exit(1);
    }

    // Configure the server address
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to a well-known address
    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        perror("Error in binding");
        exit(1);
    }

    printf("Registration server is running and listening on port %d\n", PORT);

    while (1) {
        char buffer[1024];
        char response[1024];
        memset(buffer, '\0', sizeof(buffer));
        memset(response, '\0', sizeof(response));
        addr_size = sizeof(cliAddr);
        struct Student student;

        // Receive registration details from client
        ssize_t bytesRead = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                     (struct sockaddr*)&cliAddr, &addr_size);
        if (bytesRead <= 0) {
            printf("Error in receiving data.\n");
            exit(1);
        }

        // Create a new process (slave) to handle the client request
        pid_t pid = fork();
        if (pid < 0) {
            perror("Failed to create a new process");
            exit(1);
        }

        /*********************HANDLE THE CLIENT REQUEST START**************************/
        // Slave process handles client request
        else if(pid == 0){
            printf("\nReceived request from %s:%d: \n%s\n",
                inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port), buffer);  

            // Parse the received registration details
            sscanf(buffer, "Serial Number: %[^,], Registration Number: %[^,], Name: %[^\n]",
                student.serialNumber, student.regNumber, student.name);  

            // Process request
            // Validate the registration format
            if (!validateRegistrationFormat(&student)) {
                sendto(sockfd, "Invalid registration format.", strlen("Invalid registration format."), 0,
                    (struct sockaddr*)&cliAddr, sizeof(cliAddr));
                continue;
            }

            // Check for duplicate serial number
            if (isSerialNumberDuplicate(&student)) {
                sendto(sockfd, "Duplicate serial number found.", strlen("Duplicate serial number found."), 0,
                    (struct sockaddr*)&cliAddr, sizeof(cliAddr));
                continue;
            }

            // Check for duplicate registration number
            if (isRegistrationNumberDuplicate(&student)) {
                sendto(sockfd, "Duplicate registration number found.", strlen("Duplicate registration number found."), 0,
                    (struct sockaddr*)&cliAddr, sizeof(cliAddr));
                continue;
            }

            // Open the file in append mode
            file = fopen(FILENAME, "a");
            if (file == NULL) {
                perror("Error in opening file");
                exit(1);
            }

            // Write registration details to the file
            fprintf(file, "%s\t%s\t%s\n", student.serialNumber, student.regNumber, student.name);

            // Close the file
            fclose(file);

            // Formulate response for successful registration
            snprintf(response, sizeof(response), "Registration details saved successfully");

            // Send a confirmation message to the client
            ssize_t bytesWritten = sendto(sockfd, response, strlen(response), 0,
                                        (struct sockaddr*)&cliAddr, sizeof(cliAddr));
            if (bytesWritten <= 0) {
                perror("Failed to send reply");
                continue;
            }

            printf("Sent response to %s:%d: \n%s\n\n",inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port), response);               

            // Slave process exits after handling the client request
            exit(0);        
        }
        /*********************HANDLE THE CLIENT REQUEST END**************************/ 

    }

    // Close server socket
    close(sockfd);
    return 0;
}
