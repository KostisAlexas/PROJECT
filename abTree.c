#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct Node {
    Date date;
    long long int value;
    struct Node *children[513];
    int child_count;
} Node;

int compare_dates(Date d1, Date d2) {
    if (d1.year != d2.year) {
        return d1.year - d2.year;
    }
    if (d1.month != d2.month) {
        return d1.month - d2.month;
    }
    return d1.day - d2.day;
}

Date parse_date(const char *date_str) {
    Date d;
    sscanf(date_str, "%d/%d/%d", &d.day, &d.month, &d.year);
    return d;
}

Node *create_node(Date date, long long int value) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->date = date;
    node->value = value;
    node->child_count = 0;
    return node;
}

void insert(Node **root, Date date, long long int value) {
    if (*root == NULL) {
        *root = create_node(date, value);
        return;
    }

    int cmp = compare_dates(date, (*root)->date);
    if (cmp < 0) {
        insert(&(*root)->children[0], date, value);
    } else if (cmp > 0) {
        insert(&(*root)->children[(*root)->child_count], date, value);
    } else {
        (*root)->value = value;
    }

    if ((*root)->child_count < 512) {
        (*root)->child_count++;
    } else {
        // Rebalance the tree (omitted for brevity)
    }
}

Node *search(Node *root, Date date) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = compare_dates(date, root->date);
    if (cmp == 0) {
        return root;
    }

    if (cmp < 0) {
        return search(root->children[0], date);
    } else {
        return search(root->children[root->child_count - 1], date);
    }
}

void read_csv_and_insert(const char *file_path, Node **root) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        return;
    }

    char line[1024];
    fgets(line, sizeof(line), file); // Skip header line

    while (fgets(line, sizeof(line), file)) {
        char date_str[11];
        long long int value;
        sscanf(line, "%*[^,],%*d,%[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%lld", date_str, &value);
        Date date = parse_date(date_str);
        insert(root, date, value);
    }

    fclose(file);
}

void free_tree(Node *root) {
    if (root == NULL) {
        return;
    }

    for (int i = 0; i < root->child_count; i++) {
        free_tree(root->children[i]);
    }
    free(root);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <Records.csv>\n", argv[0]);
        return 1;
    }

    Node *root = NULL;
    read_csv_and_insert(argv[1], &root);

    // Perform queries on the tree, e.g., search for a specific date
    Date target_date = {11, 4, 2023};
    Node *result = search(root, target_date);
    if (result != NULL) {
        printf("Date: %02d/%02d/%04d - Value: %lld\n", result->date.day, result->date.month, result->date.year, result->value);
    } else {
        printf("Date not found in the tree.\n");
    }

    free_tree(root);

    return 0;
}

