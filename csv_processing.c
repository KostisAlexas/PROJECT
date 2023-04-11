#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the number of columns and maximum buffer size
#define NUM_COLUMNS 11
#define BUFFER_SIZE 1024

// Function prototypes
void read_csv(const char *filename);
void write_csv(const char *filename);
void delete_record(const char *filename, int record_number);

int main() {
    const char *filename = "Records.csv";
    int choice, record_number;

    while (1) {
        printf("\n1. Read CSV\n2. Write CSV\n3. Delete record\n4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                read_csv(filename);
                break;
            case 2:
                write_csv(filename);
                break;
            case 3:
                printf("Enter the record number to delete: ");
                scanf("%d", &record_number);
                delete_record(filename, record_number);
                break;
            case 4:
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }

    return 0;
}

void read_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, file)) {
        printf("%s", buffer);
    }

    fclose(file);
}

void write_csv(const char *filename) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    char data[NUM_COLUMNS][BUFFER_SIZE];
    printf("Enter the data for each column:\n");
    for (int i = 0; i < NUM_COLUMNS; i++) {
        printf("Column %d: ", i + 1);
        scanf(" %[^\n]", data[i]); // Read until newline
        fprintf(file, "%s", data[i]);
        if (i < NUM_COLUMNS - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fclose(file);
}

void delete_record(const char *filename, int record_number) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    FILE *temp_file = fopen("temp.csv", "w");
    if (!temp_file) {
        printf("Error: Unable to open temporary file.\n");
        fclose(file);
        return;
    }

    char buffer[BUFFER_SIZE];
    int current_record = 0;
    while (fgets(buffer, BUFFER_SIZE, file)) {
        if (current_record != record_number) {
            fprintf(temp_file, "%s", buffer);
        }
        current_record++;
    }

    fclose(file);
    fclose(temp_file);

    remove(filename);
    rename("temp.csv", filename);
}
