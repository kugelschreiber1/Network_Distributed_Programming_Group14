#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_NAME_LENGTH 50
#define MAX_RECORDS 1000

struct Record
{
    int sequence_num;
    char admission_num[15];
    char name[MAX_NAME_LENGTH];
};

int is_duplicate(struct Record records[], int num_records, int sequence_num, const char admission_num[])
{
    for (int i = 0; i < num_records; i++)
    {
        if (records[i].sequence_num == sequence_num || strcmp(records[i].admission_num, admission_num) == 0)
        {
            return 1; // found duplicate
        }
    }
    return 0; // no duplicate found
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);

    struct Record records[MAX_RECORDS];
    int num_records = 0;

    FILE *fp = fopen("records.txt", "a+");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    // read existing records from file
    while (!feof(fp))
    {
        struct Record record;
        fscanf(fp, "%d,%[^,],%[^\n]", &record.sequence_num, record.admission_num, record.name);
        if (!is_duplicate(records, num_records, record.sequence_num, record.admission_num))
        {
            records[num_records] = record;
            num_records++;
        }
    }

    // create socket
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket creation failed");
        return 1;
    }

    // bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        return 1;
    }

    // listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        return 1;
    }

    // handle incoming records
    while (1)
    {
        printf("Waiting for incoming connections from a client...\n");

        // accept connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("accept failed");
            return 1;
        }

        // read record from client
        struct Record new_record;
        read(new_socket, &new_record, sizeof(new_record));

        // check for duplicates and save record to file
        int is_record_duplicate = is_duplicate(records, num_records, new_record.sequence_num, new_record.admission_num);
        if ( is_record_duplicate == 1)
        {
            printf("Duplicate record found! Not saving record.\n");
        }
        else
        {
            records[num_records] = new_record;
            num_records++;
            // save record to file
            fprintf(fp, "%d,%s,%s\n", new_record.sequence_num, new_record.admission_num, new_record.name);
            fflush(fp);

            printf("Record saved successfully.\n");
        }

        // assign the duplicate record check results to the response then send response to the client
        int response = is_record_duplicate;
        write(new_socket, &response, sizeof(response));

        // close connection
        close(new_socket);
    }

    fclose(fp);
    return 0;
}
