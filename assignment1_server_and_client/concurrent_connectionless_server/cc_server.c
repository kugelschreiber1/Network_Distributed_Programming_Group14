// To convert the code into a concurrent connectionless server, we need to make the following changes:

// Change the protocol from TCP to UDP.
// Remove the code that accepts incoming connections and creates new sockets.
// Modify the code to handle incoming packets using the recvfrom function.
// Modify the code to send response packets using the sendto function.
// Modify the code to handle multiple clients concurrently by running the record handling logic in a loop.

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX_NAME_LENGTH 50
#define MAX_RECORDS 1000

struct Record {
    int sequence_num;
    char admission_num[15];
    char name[MAX_NAME_LENGTH];
};

int is_duplicate(struct Record records[], int num_records, int sequence_num, const char admission_num[]) {
    for (int i = 0; i < num_records; i++) {
        if (records[i].sequence_num == sequence_num || strcmp(records[i].admission_num, admission_num) == 0) {
            return 1; // found duplicate
        }
    }
    return 0; // no duplicate found
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);

    struct Record records[MAX_RECORDS];
    int num_records = 0;

    FILE *fp = fopen("records.txt", "a+");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    fseek(fp, 0L, SEEK_END);
    if (ftell(fp) == 0) {
        fprintf(fp, "SNo\tAdmissionNo\tName\n");
        fflush(fp);
    } else {
        fseek(fp, 0L, SEEK_SET);
    }

    while (!feof(fp)) {
        struct Record record;
        fscanf(fp, "%d,%[^,],%[^\n]", &record.sequence_num, record.admission_num, record.name);
        if (!is_duplicate(records, num_records, record.sequence_num, record.admission_num)) {
            records[num_records] = record;
            num_records++;
        }
    }

    int server_fd;
    struct sockaddr_in address;

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("socket creation failed");
        return 1;
    }

    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return 1;
    }

    printf("Waiting for incoming requests...\n");

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_addrlen = sizeof(client_address);

        struct Record new_record;
        ssize_t recv_len = recvfrom(server_fd, &new_record, sizeof(new_record), 0, (struct sockaddr *)&client_address, &client_addrlen);

        if (recv_len < 0) {
            continue;
        }

        int is_record_duplicate = is_duplicate(records, num_records, new_record.sequence_num, new_record.admission_num);
        if (is_record_duplicate == 1)
        {
            printf("Duplicate record found! Not saving record.\n");
        }
        else
        {
            records[num_records] = new_record;
            num_records++;
            // save record to file
            fprintf(fp, "%d\t%s\t%s\n", new_record.sequence_num, new_record.admission_num, new_record.name);
            fflush(fp);
            printf("Record saved successfully.\n");
        }
        // assign the duplicate record check results to the response then send response to the client
        int response = is_record_duplicate;
        sendto(server_fd, &response, sizeof(response), 0, (struct sockaddr *)&client_address, client_addrlen);
    } // end of while loop

fclose(fp);
return 0;
}    