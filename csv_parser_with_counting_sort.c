#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_RECORDS 111438
#define BUFFER_SIZE 4096
#define FILENAME_SIZE 256

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


// Counting Sort function
void CountingSort(Record A[], Record B[], int n, long long k) {
    long long *C = (long long *)calloc(k + 1, sizeof(long long));

    for (int i = 0; i < n; i++) {
        C[A[i].value]++;
    }

    for (long long i = 1; i <= k; i++) {
        C[i] += C[i - 1];
    }

    for (int j = n - 1; j >= 0; j--) {
        B[C[A[j].value] - 1] = A[j];
        C[A[j].value]--;
    }

    free(C);
}

// Existing code for parsing CSV file
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

// Function to print records
void print_records(Record records[], int record_count) {
    for (int i = 0; i < record_count; i++) {
        printf("%s, %d, %s, %s, %s, %s, %s, %s, %lld, %lld\n",
               records[i].direction, records[i].year, records[i].date,
               records[i].weekday, records[i].country, records[i].commodity,
               records[i].transport_mode, records[i].measure, records[i].value,
               records[i].cumulative);
    }
}

int main() {
    char filename[FILENAME_SIZE];
    printf("Enter the CSV file path: ");
    scanf("%255s[^\n]", filename);  // Use scanf instead of fgets

    Record records[MAX_RECORDS];
    int record_count = 0;

    parse_csv(filename, records, &record_count);

    // Find the maximum value in the Value field
    long long max_value = LLONG_MIN;
    for (int i = 0; i < record_count; i++) {
        if (records[i].value > max_value) {
            max_value = records[i].value;
        }
    }

    // Apply Counting Sort on the Value field
    Record sorted_records[MAX_RECORDS];
    CountingSort(records, sorted_records, record_count, max_value);

    printf("Parsed and sorted %d records.\n", record_count);

    // Print the sorted records
    printf("Sorted records:\n");
    print_records(sorted_records, record_count);

    return 0;
}
