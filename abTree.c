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
    memset(node->children, 0, sizeof(node->children));
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
        if ((*root)->child_count < 512) {
            (*root)->child_count++;
        }
    } else {
        (*root)->value = value;
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

    int index = cmp < 0 ? 0 : root->child_count - 1;
    return search(root->children[index], date);
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

void in_order_traversal(Node *root) {
    if (root == NULL) {
        return;
    }

    in_order_traversal(root->children[0]);

    printf("----- Record -----\n");
    printf("Date: %02d/%02d/%04d\n", root->date.day, root->date.month, root->date.year);
    printf("Value: %lld\n", root->value);
    printf("------------------\n");

    for (int i = 1; i < root->child_count; i++) {
        in_order_traversal(root->children[i]);
    }
}

void modify_value(Node *root, Date target_date, long long int new_value) {
    Node *result = search(root, target_date);
    if (result != NULL) {
        result->value = new_value;
        printf("Value updated successfully.\n");
    } else {
        printf("Date not found in the tree.\n");
    }
}

Node *find_min(Node *root) {
    while (root->children[0] != NULL) {
        root = root->children[0];
    }
    return root;
}

Node *delete_node(Node *root, Date target_date) {
    if (root == NULL) {
        return root;
    }

    int cmp = compare_dates(target_date, root->date);

    if (cmp < 0) {
        root->children[0] = delete_node(root->children[0], target_date);
    } else if (cmp > 0) {
        for (int i = 1; i < root->child_count; i++) {
            root->children[i] = delete_node(root->children[i], target_date);
        }
    } else {
        if (root->child_count == 1) {
            Node *temp = root->children[0];
            free(root);
            return temp;
        }

        Node *temp = find_min(root->children[root->child_count - 1]);
        root->date = temp->date;
        root->value = temp->value;
        root->children[root->child_count - 1] = delete_node(root->children[root->child_count - 1], temp->date);
    }

    return root;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <Records.csv>\n", argv[0]);
        return 1;
    }

    Node *root = NULL;
    read_csv_and_insert(argv[1], &root);

    int choice;
    do {
        printf("Menu:\n");
        printf("1. Display tree (in-order traversal)\n");
        printf("2. Search value by date\n");
        printf("3. Modify value by date\n");
        printf("4. Delete record by date\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("In-order traversal of the tree:\n");
                in_order_traversal(root);
                break;
            case 2:
                {
                    printf("Enter the date (dd/mm/yyyy): ");
                    Date target_date;
                    scanf("%d/%d/%d", &target_date.day, &target_date.month, &target_date.year);

                    Node *result = search(root, target_date);
                    if (result != NULL) {
                        printf("Date: %02d/%02d/%04d - Value: %lld\n", result->date.day, result->date.month, result->date.year, result->value);
                    } else {
                        printf("Date not found in the tree.\n");
                    }
                }
                break;
            case 3:
                {
                    printf("Enter the date (dd/mm/yyyy) and new value: ");
                    Date target_date;
                    long long int new_value;
                    scanf("%d/%d/%d %lld", &target_date.day, &target_date.month, &target_date.year, &new_value);

                    modify_value(root, target_date, new_value);
                }
                break;
            case 4:
                {
                    printf("Enter the date (dd/mm/yyyy) to delete: ");
                    Date target_date;
                    scanf("%d/%d/%d", &target_date.day, &target_date.month, &target_date.year);

                    root = delete_node(root, target_date);
                    printf("Record deleted successfully.\n");
                }
                break;
            case 5:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
        printf("\n");
    } while (choice != 5);

    free_tree(root);

    return 0;
}