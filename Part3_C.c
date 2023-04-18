#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUCKET_COUNT 11
#define MAX_LINE_LENGTH 1024

typedef struct ValueNode {
    long value;
    int index;
    struct ValueNode *next;
} ValueNode;

typedef struct Node {
    char date[11];
    ValueNode *values;
    struct Node *next;
} Node;

// Function declarations
int hash_function(const char *date);
ValueNode *create_value_node(long value, int index);
Node *create_node(const char *date);
Node *find_node(Node *hash_table[], const char *date);
void insert(Node *hash_table[], const char *date, long value, int index);
void print_values(Node *hash_table[], const char *date);
void modify_value(Node *hash_table[], const char *date, long old_value, long new_value);
void delete_record(Node *hash_table[], const char *date);
void free_hash_table(Node *hash_table[]);
void show_menu();

int main() {
    const char *csv_file = "C:/Users/user/Desktop/PROJECT/Records.csv";
    Node *hash_table[BUCKET_COUNT] = {NULL};

    FILE *file = fopen(csv_file, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", csv_file);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, file); // Skip the header

    int index = 1; // Start from 1 as the first line is the header
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char date[11];
        long value;
        sscanf(line, "%*[^,],%*[^,],%[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%ld", date, &value);
       insert(hash_table, date, value, index);
    index++;
    }

    fclose(file);

    int choice;
    char input_date[11];
    long old_value, new_value;

    while (1) {
        show_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter a date (DD/MM/YYYY): ");
                scanf("%10s", input_date);
                print_values(hash_table, input_date);
                break;
            case 2:
                printf("Enter a date (DD/MM/YYYY): ");
                scanf("%10s", input_date);
                printf("Enter the old value: ");
                scanf("%ld", &old_value);
                printf("Enter the new value: ");
                scanf("%ld", &new_value);
                modify_value(hash_table, input_date, old_value, new_value);
                break;
            case 3:
                printf("Enter a date (DD/MM/YYYY) to delete a record: ");
                scanf("%10s", input_date);
                delete_record(hash_table, input_date);
                break;
            case 4:
                printf("Exiting...\n");
                free_hash_table(hash_table);
                return 0;
            default:
                printf("Invalid option! Please try again.\n");
        }
    }

    return 0;
}

int hash_function(const char *date) {
    int sum = 0;
    for (int i = 0; date[i] != '\0'; i++) {
        sum += date[i];
    }
    return sum % BUCKET_COUNT;
}

ValueNode *create_value_node(long value, int index) {
    ValueNode *new_value_node = (ValueNode *) malloc(sizeof(ValueNode));
    new_value_node->value = value;
    new_value_node->index = index;
    new_value_node->next = NULL;
    return new_value_node;
}


Node *create_node(const char *date) {
    Node *new_node = (Node *) malloc(sizeof(Node));
    strcpy(new_node->date, date);
    new_node->values = NULL;
    new_node->next = NULL;
    return new_node;
}

Node *find_node(Node *hash_table[], const char *date) {
    int index = hash_function(date);
    Node *current = hash_table[index];
    while (current != NULL) {
        if (strcmp(current->date, date) == 0) {
            return current;
}
current = current->next;
}
return NULL;
}

void insert(Node *hash_table[], const char *date, long value, int index) {
    Node *node = find_node(hash_table, date);
    if (node == NULL) {
        int index = hash_function(date);
        node = create_node(date);
        node->next = hash_table[index];
        hash_table[index] = node;
}
ValueNode *value_node = create_value_node(value, index);
    value_node->next = node->values;
    node->values = value_node;
}

void print_values(Node *hash_table[], const char *date) {
    Node *node = find_node(hash_table, date);
    if (node != NULL) {
        printf("\nValues for %s:\n", date);
        printf("%-10s %-10s\n", "Index", "Value");
        printf("--------------------\n");

        ValueNode *current = node->values;
        ValueNode *prev = NULL;
        ValueNode *next = NULL;

        while (current != NULL) {
            next = current->next;
            current->next = prev;
            prev = current;
            current = next;
        }

        current = prev;
        while (current != NULL) {
            printf("%-10d %-10ld\n", current->index, current->value);
            current = current->next;
        }
    } else {
        printf("No data found for %s\n", date);
    }
}


void modify_value(Node *hash_table[], const char *date, long old_value, long new_value) {
    Node *node = find_node(hash_table, date);
    if (node != NULL) {
        ValueNode *current = node->values;
        int modified = 0;
    while (current != NULL) {
        if (current->value == old_value) {
            current->value = new_value;
            modified = 1;
        break;
    }
    current = current->next;
    }
    if (modified) {
        printf("Value modified successfully.\n");
    } else {
        printf("No matching value found for %s\n", date);
    }
    } else {
        printf("No data found for %s\n", date);
    }
}

void delete_record(Node *hash_table[], const char *date) {
int index = hash_function(date);
Node *current = hash_table[index];
Node *prev = NULL;
while (current != NULL) {
    if (strcmp(current->date, date) == 0) {
        if (prev == NULL) {
            hash_table[index] = current->next;
    } else {
            prev->next = current->next;
    }
ValueNode *value_node = current->values;
    while (value_node != NULL) {
        ValueNode *next_value = value_node->next;
        free(value_node);
        value_node = next_value;
        }
    free(current);
    printf("Record deleted successfully.\n");
    return;
        }
        prev = current;
        current = current->next;
        }
    printf("No data found for %s\n", date);
}

void free_hash_table(Node *hash_table[]) {
    for (int i = 0; i < BUCKET_COUNT; i++) {
        Node *current = hash_table[i];
        while (current != NULL) {
            Node *next_node = current->next;
            ValueNode *value_node = current->values;
            while (value_node != NULL) {
                ValueNode *next_value = value_node->next;
                free(value_node);
                value_node = next_value;
            }
                free(current);
                current = next_node;
        }
    }
}

void show_menu() {
    printf("\n+----------------------------------+\n");
    printf("| Choose an option:                |\n");
    printf("| 1) Search values for a date      |\n");
    printf("| 2) Modify a record's value       |\n");
    printf("| 3) Delete a record by date       |\n");
    printf("| 4) Exit the program              |\n");
    printf("+----------------------------------+\n");
    printf("Enter your choice: ");
}