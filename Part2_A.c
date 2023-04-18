#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for the Value field
typedef struct ValueNode {
    long long value;
    struct ValueNode *next;
} ValueNode;

// Structure for the AVL tree node
typedef struct TreeNode {
    char date[11];
    ValueNode *values;
    int height;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// Function to create a new ValueNode
ValueNode *newValueNode(long long value) {
    ValueNode *node = (ValueNode *)malloc(sizeof(ValueNode)); //Notice Dynamic memory allocation
    node->value = value;
    node->next = NULL;
    return node;
}

// Function to create a new TreeNode
TreeNode *newTreeNode(char *date, long long value) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode)); //Notice malloc here too!
    strncpy(node->date, date, 11);                         //Now we can grow and shrink the tree during runtime!
    node->values = newValueNode(value);
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

// Function to insert a value into the ValueNode list
void insertValue(ValueNode **head, long long value) {
    ValueNode *new_node = newValueNode(value);
    new_node->next = *head;
    *head = new_node;
}

// Function to insert a record into the AVL tree
TreeNode *insert(TreeNode *node, char *date, long long value) {
    if (node == NULL) {
        return newTreeNode(date, value);
    }

    if (strcmp(date, node->date) < 0) {
        node->left = insert(node->left, date, value);
    } else if (strcmp(date, node->date) > 0) {
        node->right = insert(node->right, date, value);
    } else {
        insertValue(&(node->values), value);
        return node;
    }

    updateHeight(node);

    int balance = getBalance(node);

    if (balance > 1) {
        if (strcmp(date, node->left->date) < 0) {
            return rotateRight(node);
        } else {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
    }

    if (balance < -1){
        if (strcmp(date, node->right->date) > 0) {
            return rotateLeft(node);
        } else {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
    }

    return node;
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


// Function to print the linked list of values
void printValues(ValueNode *head) {
    ValueNode *current = head;
    while (current != NULL) {
        printf("%lld ", current->value);
        current = current->next;
    }
}

// Function for in-order traversal and printing the AVL tree
void inOrderTraversal(TreeNode *node) {
    if (node == NULL) {
        return;
    }

    inOrderTraversal(node->left);
    printf("Date: %s, Values: ", node->date);
    printValues(node->values);
    printf("\n");
    inOrderTraversal(node->right);
}

// Function to search for a date in the AVL tree and return its TreeNode
TreeNode *search(TreeNode *node, char *date) {
    if (node == NULL || strcmp(date, node->date) == 0) {
        return node;
    }

    if (strcmp(date, node->date) < 0) {
        return search(node->left, date);
    }

    return search(node->right, date);
}

// Function to modify a value of a date specified by the user
void modifyValue(TreeNode *node, char *date, long long oldValue, long long newValue) {
    TreeNode *foundNode = search(node, date);
    if (foundNode == NULL) {
        printf("Date not found.\n");
        return;
    }

    ValueNode *current = foundNode->values;
    while (current != NULL) {
        if (current->value == oldValue) {
            current->value = newValue;
            printf("Value modified successfully. Search AVL to confirm\n");
            return;
        }
        current = current->next;
    }

    printf("Value not found.\n");
}

// Function to delete a value from the ValueNode list and return the new head
ValueNode *deleteValue(ValueNode *head, long long value) {
    ValueNode *current = head;
    ValueNode *prev = NULL;

    while (current != NULL && current->value != value) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return head;
    }

    if (prev == NULL) {
        head = head->next;
    } else {
        prev->next = current->next;
    }

    free(current);
    return head;
}

// Function to delete a value from a date specified by the user
void deleteValueByDate(TreeNode *node, char *date, long long value) {
    TreeNode *foundNode = search(node, date);
    if (foundNode == NULL) {
        printf("Date not found.\n");
        return;
    }

    foundNode->values = deleteValue(foundNode->values, value);
    printf("Value deleted successfully. Search AVL to confirm.\n");
}

// Function to display the menu and handle user input
void menu(TreeNode *root) {
    int choice;
    char date[11];
    long long oldValue, newValue;

    while (1) {
        printf("\nMenu:\n");
        printf("1. Display AVL tree with in-order traversal\n");
        printf("2. Search for a value by date\n");
        printf("3. Modify a value by date\n");
        printf("4. Delete a value by date\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("In-order traversal:\n");
                printf("Date       Values\n");
                printf("------------ ----------------------------------------\n");
                inOrderTraversal(root);
                break;
            case 2:
                printf("Enter the date (dd/mm/yyyy) to search: ");
                scanf("%s", date);
                TreeNode *foundNode = search(root, date);
                if (foundNode != NULL) {
                    printf("Values for %s: ", date);
                    printValues(foundNode->values);
                    printf("\n");
                } else {
                    printf("Date not found.\n");
                }
                break;
            case 3:
                printf("Enter the date (dd/mm/yyyy) to modify a value: ");
                scanf("%s", date);
                printf("Enter the old value: ");
                scanf("%lld", &oldValue);
                printf("Enter the new value: ");
                scanf("%lld", &newValue);
                modifyValue(root, date, oldValue, newValue);
                break;
            case 4:
                printf("Enter the date (dd/mm/yyyy) to delete a value: ");
                scanf("%s", date);
                printf("Enter the value to delete: ");
                scanf("%lld", &oldValue);
                deleteValueByDate(root, date, oldValue);
                break;
            case 5:
                printf("Exiting the program.\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}


// Function to free the AVL tree
void freeTree(TreeNode *node) {
    if (node == NULL) {
        return;
    }

    freeTree(node->left);
    freeTree(node->right);

    ValueNode *current = node->values;
    ValueNode *temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }

    free(node);
}



int main() {

    //hardcoded file
const char *filename = "C:/Users/user/Desktop/PROJECT/Records.csv"; 

    TreeNode *root = buildAVLTree(filename);

    // Perform any desired operations on the AVL tree here

    printf("\nAVL complete\n");

    menu(root);

    freeTree(root);
    return 0;
}