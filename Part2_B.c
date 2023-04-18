#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Structure for the DateNode
typedef struct DateNode {
    char date[11];
    struct DateNode *next;
} DateNode;

// Structure for the AVL tree node
typedef struct TreeNode {
    long long value;
    DateNode *dates;
    int height;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// Function to create a new DateNode
DateNode *newDateNode(char *date) {
    DateNode *node = (DateNode *)malloc(sizeof(DateNode));
    strncpy(node->date, date, 11);
    node->next = NULL;
    return node;
}

// Function to create a new TreeNode
TreeNode *newTreeNode(char *date, long long value) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->value = value;
    node->dates = newDateNode(date);
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Function to get the height of a TreeNode
int height(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

// Function to get the balance factor of a TreeNode
int getBalance(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

// Function to update the height of a TreeNode
void updateHeight(TreeNode *node) {
    node->height = 1 + (height(node->left) > height(node->right) ? height(node->left) : height(node->right));
}

// Function to rotate a TreeNode right
TreeNode *rotateRight(TreeNode *y) {
    TreeNode *x = y->left;
    TreeNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

// Function to rotate a TreeNode left
TreeNode *rotateLeft(TreeNode *x) {
    TreeNode *y = x->right;
    TreeNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

// Function to insert a date into the DateNode list
void insertDate(DateNode **head, char *date) {
    DateNode *new_node = newDateNode(date);
    new_node->next = *head;
    *head = new_node;
}

// Function to insert a record into the AVL tree
TreeNode *insert(TreeNode *node, char *date, long long value) {
    if (node == NULL) {
        return newTreeNode(date, value);
    }

    if (value < node->value) {
        node->left = insert(node->left, date, value);
    } else if (value > node->value) {
        node->right = insert(node->right, date, value);
    } else {
        insertDate(&(node->dates), date);
        return node;
    }

    updateHeight(node);

    int balance = getBalance(node);

    if (balance > 1) {
        if (value < node->left->value) {
            return rotateRight(node);
       } else {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
            }
}

if (balance < -1) {
    if (value > node->right->value) {
        return rotateLeft(node);
    } else {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
}

return node;
}

// Function to find the TreeNode with the minimum value
TreeNode *minValueNode(TreeNode *node) {
TreeNode *current = node;
while (current->left != NULL) {
current = current->left;
}
return current;
}

// Function to find the TreeNode with the maximum value
TreeNode *maxValueNode(TreeNode *node) {
TreeNode *current = node;
while (current->right != NULL) {
current = current->right;
}
return current;
}

// Function to read the CSV file and build the AVL tree
TreeNode *buildAVLTree(const char *filename) {
FILE *file = fopen(filename, "r");
if (!file) {
fprintf(stderr, "Failed to open the file.\n");
return NULL;
}

char buffer[1024];
fgets(buffer, sizeof(buffer), file); // Skip the header row

TreeNode *root = NULL;
char date[11];
long long value;

while (fgets(buffer, sizeof(buffer), file)) {
    sscanf(buffer, "%*[^,],%*[^,],%[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%lld", date, &value);
    root = insert(root, date, value);
}

fclose(file);
return root;

}

// Function to free the AVL tree
void freeTree(TreeNode *node) {
if (node == NULL) {
return;
}

freeTree(node->left);
freeTree(node->right);

DateNode *current = node->dates;
DateNode *temp;
while (current != NULL) {
    temp = current;
    current = current->next;
    free(temp);
}

free(node);

}

// print 10 Days in a row in an easy to read way
void printDates(DateNode *dates) {
    DateNode *current = dates;
    int count = 0;

    while (current != NULL) {
        printf("%-12s", current->date);
        count++;
        if (count % 10 == 0) {
            printf("\n");
        }
        current = current->next;
    }
    if (count % 10 != 0) {
        printf("\n");
    }
}


void menu(TreeNode *root) {
    printf("\n");
    printf("============================================\n");
    printf("               MENU OPTIONS                 \n");
    printf("============================================\n");
    printf("  1. Find the day/days with the smallest value\n");
    printf("  2. Find the day/days with the largest value\n");
    printf("  3. Exit\n");
    printf("============================================\n");
    printf("Enter your choice (1-3): ");

    int option;
    scanf("%d", &option);

    switch (option) {
        case 1: {
            TreeNode *minNode = minValueNode(root);
            printf("\nThe smallest value: %lld\n", minNode->value);
            printf("The day/days with the smallest value:\n");
            printDates(minNode->dates);
            menu(root);
            break;
        }
        case 2: {
            TreeNode *maxNode = maxValueNode(root);
            printf("\nThe largest value: %lld\n", maxNode->value);
            printf("The day/days with the largest value:\n");
            printDates(maxNode->dates);
            menu(root);
            break;
        }
        case 3:
            return;
        default:
            printf("Invalid option. Please try again.\n");
            menu(root);
    }
}


int main() {
// hardcoded file
const char *filename = "C:/Users/user/Desktop/PROJECT/Records.csv";

TreeNode *root = buildAVLTree(filename);

printf("\nAVL complete\n");

menu(root);

freeTree(root);
return 0;
}