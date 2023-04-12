#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

long int find_max_value(CsvRecord *records, int size) {
    long int max_value = 0;
    for (int i = 0; i < size; ++i) {
        if (records[i].value > max_value) {
            max_value = records[i].value;
        }
    }
    return max_value;
}

void counting_sort(CsvRecord *records, int size) {
    long int max_value = find_max_value(records, size);
    int *count = calloc(max_value + 1, sizeof(int));

    for (int i = 0; i < size; ++i) {
        count[records[i].value]++;
    }

    for (int i = 1; i <= max_value; ++i) {
        count[i] += count[i - 1];
    }

    CsvRecord *sorted_records = malloc(size * sizeof(CsvRecord));
    for (int i = size - 1; i >= 0; --i) {
        sorted_records[count[records[i].value] - 1] = records[i];
        count[records[i].value]--;
    }

    memcpy(records, sorted_records, size * sizeof(CsvRecord));
    free(sorted_records);
    free(count);
}

void merge(CsvRecord *records, int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    CsvRecord *L = malloc(n1 * sizeof(CsvRecord));
    CsvRecord *R = malloc(n2 * sizeof(CsvRecord));

    for (i = 0; i < n1; i++)
        L[i] = records[left + i];
    for (j = 0; j < n2; j++)
        R[j] = records[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (L[i].value <= R[j].value) {
            records[k] = L[i];
            i++;
        } else {
            records[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        records[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        records[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void merge_sort(CsvRecord *records, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        merge_sort(records, left, mid);
        merge_sort(records, mid + 1, right);

        merge(records, left, mid, right);
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

    printf("Select the sorting algorithm (1 - Counting Sort, 2 - Merge Sort): ");
    scanf("%d", &sort_choice);

    clock_t start = clock();
    if (sort_choice == 1) {
        counting_sort(records, size);
    } else if (sort_choice == 2) {
        merge_sort(records, 0, size - 1);
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