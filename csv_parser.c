#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORDS 111438
#define BUFFER_SIZE 4096

typedef struct {
    char direction[20];
    int year;
    char date[11];
    char weekday[10];
    char country[50];
    char commodity[50];
    char transport_mode[20];
    char measure[10];
    long long value;
    long long cumulative;
} Record;

void parse_csv(char *filename, Record records[], int *record_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file.\n");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    fgets(buffer, BUFFER_SIZE, file); // Read and ignore the header line

    while (fgets(buffer, BUFFER_SIZE, file)) {
        Record record;
        char *token = strtok(buffer, ",");
        strcpy(record.direction, token);

        token = strtok(NULL, ",");
        record.year = atoi(token);

        token = strtok(NULL, ",");
        strcpy(record.date, token);

        token = strtok(NULL, ",");
        strcpy(record.weekday, token);

        token = strtok(NULL, ",");
        strcpy(record.country, token);

        token = strtok(NULL, ",");
        strcpy(record.commodity, token);

        token = strtok(NULL, ",");
        strcpy(record.transport_mode, token);

        token = strtok(NULL, ",");
        strcpy(record.measure, token);

        token = strtok(NULL, ",");
        record.value = atoll(token);

        token = strtok(NULL, ",");
        record.cumulative = atoll(token);

        records[*record_count] = record;
        (*record_count)++;
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <csv_file>\n", argv[0]);
        exit(1);
    }

    char *filename = argv[1];
    Record records[MAX_RECORDS];
    int record_count = 0;

    parse_csv(filename, records, &record_count);

    printf("Parsed %d records.\n", record_count);

    return 0;
}
