#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libgen.h>

typedef struct {
    char direction[10];
    int year;
    char date[11];
    char weekday[10];
    char country[20];
    char commodity[100];
    char transport_mode[10];
    char measure[10];
    long int value;
    long int cumulative;
} CsvRecord;

CsvRecord *parse_line(char *line) {
    CsvRecord *record = malloc(sizeof(CsvRecord));
    sscanf(line, "%[^,],%d,%[^,],%[^,],%[^,],\"%[^\"]\",%[^,],%[^,],%ld,%ld",
           record->direction, &record->year, record->date, record->weekday,
           record->country, record->commodity, record->transport_mode,
           record->measure, &record->value, &record->cumulative);

    return record;
}

CsvRecord *parse_csv(const char *file, int *size) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return NULL;
    }

    CsvRecord *records = malloc(111438 * sizeof(CsvRecord));
    *size = 0;

    char line[512];
    fgets(line, sizeof(line), fp); // skip header

    while (fgets(line, sizeof(line), fp)) {
        CsvRecord *record = parse_line(line);
        records[(*size)++] = *record;
        free(record);
    }

    fclose(fp);
    return records;
}



void swap(CsvRecord *a, CsvRecord *b) {
    CsvRecord temp = *a;
    *a = *b;
    *b = temp;
}

void max_heapify(CsvRecord *records, int size, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < size && records[left].cumulative > records[largest].cumulative)
        largest = left;

    if (right < size && records[right].cumulative > records[largest].cumulative)
        largest = right;

    if (largest != i) {
        swap(&records[i], &records[largest]);
        max_heapify(records, size, largest);
    }
}

void heap_sort(CsvRecord *records, int size) {
    for (int i = size / 2 - 1; i >= 0; i--)
        max_heapify(records, size, i);

    for (int i = size - 1; i > 0; i--) {
        swap(&records[0], &records[i]);
        max_heapify(records, i, 0);
    }
}

int partition(CsvRecord *records, int low, int high) {
    long int pivot = records[high].cumulative;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (records[j].cumulative <= pivot) {
            i++;
            swap(&records[i], &records[j]);
        }
    }
    swap(&records[i + 1], &records[high]);
    return i + 1;
}

void quick_sort(CsvRecord *records, int low, int high) {
    if (low < high) {
        int pi = partition(records, low, high);

        quick_sort(records, low, pi - 1);
        quick_sort(records, pi + 1, high);
    }
}

void print_records(CsvRecord *records, int size) {
    for (int i = 0; i < size; ++i) {
        printf("%s: %ld\n", records[i].date, records[i].value);
    }
}

void save_records_to_file(CsvRecord *records, int size, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(fp, "Direction,Year,Date,Weekday,Country,Commodity,Transport_Mode,Measure,Value,Cumulative\n");
    for (int i = 0; i < size; ++i) {
        fprintf(fp, "%s,%d,%s,%s,%s,\"%s\",%s,%s,%ld,%ld\n",
                records[i].direction, records[i].year, records[i].date,
                records[i].weekday, records[i].country, records[i].commodity,
                records[i].transport_mode, records[i].measure, records[i].value,
                records[i].cumulative);
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {

    printf("The file is Hardcoded so you only have to input it once\n\n");

    const char *csv_file = "C:/Users/user/Desktop/PROJECT/Records.csv";
    int size;

    CsvRecord *records = parse_csv(csv_file, &size);
    if (records == NULL) {
        return 1;
    }

    int sort_choice;

    printf("Select the sorting algorithm (1 - Heap Sort, 2 - Quick Sort): ");
    scanf("%d", &sort_choice);

    clock_t start = clock();
    if (sort_choice == 1) {
        heap_sort(records, size);
    } else if (sort_choice == 2) {
        quick_sort(records, 0, size - 1);
    } else {
        printf("Invalid sorting algorithm choice. Exiting...\n\n");
        free(records);
        return 1;
    }
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Sorting took %.2f seconds.\n\n", time_spent);
    print_records(records, size);

    char choice;
    printf("Do you want to save the sorted data to a file? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
        char filename[256];
        printf("\nCreate a name for your file: ");
        scanf("%s", filename);

        char fullpath[1024];
        strncpy(fullpath, argv[0], sizeof(fullpath));
        char *dir = dirname(fullpath); // Extract directory from argv[0]
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, filename);

        save_records_to_file(records, size, fullpath);
        printf("\nData saved to %s\n\n", fullpath);
    }

    free(records);
    return 0;
}
