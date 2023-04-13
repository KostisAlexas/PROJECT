#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Total number of records in the dataset
#define NUM_RECORDS 111438

// Structure representing a record from the dataset
typedef struct {
    char direction[20];
    int year;
    time_t date;
    char weekday[10];
    char country[50];
    char commodity[100];
    char transport_mode[20];
    char measure[20];
    long long value;
    long long cumulative;
} Record;

// Structure representing an indexed record, we'll use that later to sort the results
typedef struct {
    int index;
    Record record;
} IndexedRecord;

// Global array holding the indexed records
IndexedRecord records[NUM_RECORDS];

// Function prototypes
void read_csv(const char *filename);
void sort_records_by_date();
time_t date_to_unix(const char *date_str);
void parse_date(const char *date_str, int *day, int *month, int *year);
int date_comparator(const void *a, const void *b);
int binary_interpolation_step_search(time_t target_date, int *found_indexes);
int improved_binary_interpolation_step_search(time_t target_date, int *found_indexes);
int compare_original_indexes(const void *a, const void *b);

int main() {
    // Read records from the CSV file
    read_csv("C:/Users/user/Desktop/PROJECT/Records.csv");

    // Sort records by date
    sort_records_by_date();

    // Loop until the user decides to exit
    while (1) {
        // Ask the user for the target date
        char date[11];
        printf("\nEnter a date (ex: 01/01/2015): ");
        scanf("%10s", date);
        time_t target_date = date_to_unix(date);

        // Ask the user for the search method
        int choice;
        printf("Choose a search method:\n");
        printf("1. Binary Interpolation Step Search\n");
        printf("2. Improved Binary Interpolation Step Search\n");
        printf("Enter your choice (1 or 2): ");
        scanf("%d", &choice);

        // Start the clock to measure search time
        clock_t start = clock();

        // Perform the search and store the found indexes
        int found_indexes[NUM_RECORDS];
        int num_found = 0;

        if (choice == 1) {
            num_found = binary_interpolation_step_search(target_date, found_indexes);
        } else if (choice == 2) {
            num_found = improved_binary_interpolation_step_search(target_date, found_indexes);
        } else {
            printf("Invalid choice!\n");
            return 1;
        }

        // Display the search results
        if (num_found > 0) {
            // Sort found_indexes by original index values
            qsort(found_indexes, num_found, sizeof(int), compare_original_indexes);

            // Print the results
            printf("\nDate: %s\n", date);
            printf("-------------------------------------------------\n");
            printf("%-10s %-20s %-20s\n", "File Index", "Value", "Cumulative");
            printf("-------------------------------------------------\n");
            for (int i = 0; i < num_found; i++) {
                int original_index = records[found_indexes[i]].index;
                printf("%-10d %-20lld %-20lld\n", original_index + 2, records[found_indexes[i]].record.value, records[found_indexes[i]].record.cumulative);
            }
            printf("-------------------------------------------------\n");
        } else {
            printf("\nDate not found.\n");
        }

    // Stop the clock and calculate the search time
    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Search time: %f seconds\n", cpu_time_used);

    // Ask the user if they want to search for another date
    char search_again;
    printf("Would you like to search for another date? (y/n): ");
    scanf(" %c", &search_again);
    if (search_again == 'n' || search_again == 'N') {
        break;
    }
    }

return 0;

}

// Function to perform binary interpolation step search for a target date
int binary_interpolation_step_search(time_t target_date, int *found_indexes) {
    int left = 0;
    int right = NUM_RECORDS - 1;
    int num_found = 0;

    while (left <= right && target_date >= records[left].record.date && target_date <= records[right].record.date) {
        int next = left + (((double)(right - left) / (records[right].record.date - records[left].record.date)) * (target_date - records[left].record.date));

        if (target_date == records[next].record.date) {
            // Found the target date
            found_indexes[num_found++] = next;

            // Check for other records with the same date on the left of the found position
            int temp = next - 1;
             while (temp >= 0 && records[temp].record.date == target_date) {
                 found_indexes[num_found++] = temp;
                 temp--;
            }

            // Check for other records with the same date on the right of the found position
            temp = next + 1;
            while (temp < NUM_RECORDS && records[temp].record.date == target_date) {
                found_indexes[num_found++] = temp;
                temp++;
        }
            break;
        } else {
            int i = 0;
            int step = sqrt(right - left);

            if (target_date >= records[next].record.date) {
                while (target_date > records[next + i * step].record.date) {
                    i++;
                }
                right = next + i * step;
            } else {
                while (target_date < records[next - i * step].record.date) {
                    i++;
                }
                left = next - (i - 1) * step;
                right = next - i * step;
            }
        }
    }

    return num_found;
}

// Function to perform improved binary interpolation step search for a target date
int improved_binary_interpolation_step_search(time_t target_date, int *found_indexes) {
    int left = 0;
    int right = NUM_RECORDS - 1;
    int num_found = 0;

    while (left <= right && target_date >= records[left].record.date && target_date <= records[right].record.date) {
        int next = left + (((double)(right - left) / (records[right].record.date - records[left].record.date)) * (target_date - records[left].record.date));

        if (target_date == records[next].record.date) {
            // Found the target date
            found_indexes[num_found++] = next;

            // Check for other records with the same date on the left of the found position
            int temp = next - 1;
            while (temp >= 0 && records[temp].record.date == target_date) {
                found_indexes[num_found++] = temp;
                temp--;
            }

            // Check for other records with the same date on the right of the found position
            temp = next + 1;
            while (temp < NUM_RECORDS && records[temp].record.date == target_date) {
                found_indexes[num_found++] = temp;
                temp++;
            }

            break;
        } else {
            int i = 1;
            int step = sqrt(right - left);

            if (target_date >= records[next].record.date) {
                while (target_date > records[next + i * step].record.date) {
                    i = 2 * i;
                }
                right = next + i * step;
            } else {
                while (target_date < records[next - i * step].record.date) {
                    i = 2 * i;
}
                    left = next - (i - 1) * step;
                    right = next - i * step;
                }
            }
}
return num_found;

}

// Function to read the CSV file and store the records in the global array
void read_csv(const char *filename) {
FILE *file = fopen(filename, "r");
if (file == NULL) {
fprintf(stderr, "Could not open file: %s\n", filename);
exit(EXIT_FAILURE);
}
char line[1024];
fgets(line, sizeof(line), file); // Skip header

for (int i = 0; i < NUM_RECORDS && fgets(line, sizeof(line), file); i++) {
    records[i].index = i;
    char date_str[11];
    sscanf(line, "%[^,],%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%lld,%lld",
           records[i].record.direction,
           &records[i].record.year,
           date_str,
           records[i].record.weekday,
           records[i].record.country,
           records[i].record.commodity,
           records[i].record.transport_mode,
           records[i].record.measure,
           &records[i].record.value,
           &records[i].record.cumulative);
    records[i].record.date = date_to_unix(date_str);
}

fclose(file);
}

// Function to sort records by date
void sort_records_by_date() {
qsort(records, NUM_RECORDS, sizeof(IndexedRecord), date_comparator);
}

// Function to convert a date string (dd/mm/yyyy) to a UNIX timestamp
time_t date_to_unix(const char *date_str) {
struct tm tm;
memset(&tm, 0, sizeof(struct tm));
int day, month, year;
parse_date(date_str, &day, &month, &year);
tm.tm_mday = day;
tm.tm_mon = month - 1;
tm.tm_year = year - 1900;
return mktime(&tm);
}

// Function to parse a date string (dd/mm/yyyy) and return day, month, and year as integers
void parse_date(const char *date_str, int *day, int *month, int *year) {
sscanf(date_str, "%d/%d/%d", day, month, year);
}

// Date comparator function for qsort
int date_comparator(const void *a, const void *b) {
const IndexedRecord *record_a = (const IndexedRecord *)a;
const IndexedRecord *record_b = (const IndexedRecord *)b;
if (record_a->record.date < record_b->record.date) {
return -1;
} else if (record_a->record.date > record_b->record.date) {
return 1;
} else {
return 0;
}
}

// Function to compare original indexes of records
int compare_original_indexes(const void *a, const void *b) {
int index_a = *(const int *)a;
int index_b = *(const int *)b;
int original_index_a = records[index_a].index;
int original_index_b = records[index_b].index;

    if (original_index_a < original_index_b) {
        return -1;
    } else if (original_index_a > original_index_b) {
        return 1;
    } else {
        return 0;
    }

}

/* Compare integers for sorting found_indexes array */
int compare_int(const void *a, const void *b) {
int int_a = *(const int *)a;
int int_b = *(const int *)b;

if (int_a < int_b) {
    return -1;
} else if (int_a > int_b) {
    return 1;
} else {
    return 0;
}

}

/* Compare IndexedRecord structs for sorting records array */
int compare_indexed_records(const void *a, const void *b) {
const IndexedRecord *record_a = (const IndexedRecord *)a;
const IndexedRecord *record_b = (const IndexedRecord *)b;

if (record_a->record.date < record_b->record.date) {
    return -1;
} else if (record_a->record.date > record_b->record.date) {
    return 1;
} else {
    return record_a->index - record_b->index;
}

}