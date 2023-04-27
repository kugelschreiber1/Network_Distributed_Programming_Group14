#include <stdio.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_RECORDS 1000


//formatted column titles
// data structure that hols the properties of a student
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

int main()
{
    printf("Hello World");
    struct Record records[MAX_RECORDS];
    int num_records = 0;

    FILE *fp = fopen("records.txt", "a+");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    if (NULL != fp) {
    fseek (fp, 0, SEEK_END);
    int size = ftell(fp);

    if (0 == size) {
        printf("file is empty\n");
    }
}

    // check if column titles are already there for formatting
    char first_line[100];
    fgets(first_line, 100, fp);
    if (strcmp(first_line, "SequenceNo,AdmissionNo,Name\n") != 0)
    {
        // column titles not found, so add them to file
        fclose(fp);
        fp = fopen("records.txt", "w");
        fprintf(fp, "SequenceNo,AdmissionNo,Name\n");
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

    // read user input and save to file
    struct Record new_record;
    printf("Enter sequence number:\n");
    scanf("%d", &new_record.sequence_num);
    printf("Enter admission number:\n");
    scanf(" %[^\n]", new_record.admission_num);
    printf("Enter name:\n");
    scanf(" %[^\n]", new_record.name);
    int is_record_duplicate = is_duplicate(records, num_records, new_record.sequence_num, new_record.admission_num);
    if (is_record_duplicate == 1)
    {
        printf("Duplicate record found! Not saving record\n");
    }
    else
    {
        fprintf(fp, "%d,%s,%s\n", new_record.sequence_num, new_record.admission_num, new_record.name);
        printf("Record added successfully!\n");
    }

    fclose(fp);
    return 0;
}


