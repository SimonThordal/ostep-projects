#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define _GNU_SOURCE

const char* DATABASE_FILE = "database.txt";
const char* SEPARATOR=",";
typedef struct node
{
    int key;
    char* val;
    struct node* next;
} node_t;

/**
 * Given a comma separated string of arguments
 * split them into a string per argument and put them in arg_arr.
 * There can maximum be 3 arguments per string.
 * 
 * arg: a string with arguments of the form p,20,foo or g,10
 * arg_arr: a pointer to an array of char arrays
 */
int argparse(char* arg, char** arg_arr) {
    errno = 0;
    int cnt = 0;
    char *c = strtok(arg, SEPARATOR);
    while (c != NULL) {
        if (cnt > 2) {
            fprintf(stderr, "kv: illegal argument received: %s", arg);
            return -1;
        }
        arg_arr[cnt] = NULL;
        arg_arr[cnt] = (char*)malloc(sizeof(c));
        arg_arr[cnt] = c;
        cnt++;
        c = strtok(NULL, SEPARATOR);
    }
    return 0;
}

/**
 * Turn a k,v string into a node struct.
 */
int row_to_node(char* row, node_t* node) {
    errno = 0;
    char *c = strtok(row, SEPARATOR);
    if (c == NULL || *c=='\0') {
        return -1;
    }
    // Convert the key to a string
    char *endptr;
    long int key = strtol(c, &endptr, 10);
    if (errno != 0) {
        printf("kv: error encountered with errno: %c\n", errno);
        errno = 0;
        return -1;
    }
    // If we did not end at null the key included non digits
    if (*endptr != '\0') {
        return -1;
    }
    // Grab the value from the arg
    char *val = strtok(NULL, SEPARATOR);
    if (val == NULL) {
        return -1;
    }
    node->key = (int)key;
    node->val = (char*)malloc(sizeof(val));
    strcpy(node->val, val);
    node->next = NULL;
    return 0;
}

/**
 * Read in the database and return it represented as a linked list.
 */
int initialize(node_t* head, FILE *fp) {
    rewind(fp);
    errno = 0;
    size_t n;
    char* line = NULL;
    size_t len = 0;
    node_t* prev = NULL;
    while ((n = getline(&line, &len, fp)) != -1)
    {
        // Read the line into the current node
        row_to_node(line, head);
        // Add the current node to linked list if it is not the first node
        if (prev != NULL) {
            prev->next = head;
        }
        prev = head;
        // Create a new node for the next line
        node_t* node = (node_t*)malloc(sizeof(node_t));
        if (errno != 0) {
            fprintf(stderr, "kv: error reading database file");
            return -1;
        }
        head = node;
    }
    return 0;
} 


/**
 * Enter a value into the DB with a given key.
 */
int put(char* key_c, char* val, node_t *head) {
    node_t *node = (node_t*)malloc(sizeof(node_t));// Convert the key to a string
    char *endptr;
    long int key = strtol(key_c, &endptr, 10);
    if (errno != 0) {
        printf("kv: error encountered with errno: %c\n", errno);
        errno = 0;
        return -1;
    }
    // If we did not end at null the key included non digits
    if (*endptr != '\0') {
        return -1;
    }
    node->key = (int)key;
    node->val = (char*)malloc(sizeof(val));
    strcpy(node->val, val);
    node->next = NULL;
    while (head->next != NULL)
    {
        head = head->next;
    }
    head->next = node;
    return 0;
}

/**
 * List all k,v pairs in the database.
 */
void all(node_t* head) {
    while (head != NULL) {
        printf("%d:%s\n", head->key, head->val);
        head = head->next;
    }
}

/**
 * Get the value stored in a given key.
 * Expects arguments of the form g,KEY
 */
int get(char* key, node_t* head, char *res) {
    node_t* node = head;
    // Iterate through the linked list until the correct key is found
    while (node != NULL)
    {
        if (node->key == atoi(key)) {
            res = (char*)malloc(sizeof(node->val));
            res = node->val;
            return 0;
        }
        node = node->next;  
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // If no arguments are given exit
    if (argc < 2) {
        exit(EXIT_SUCCESS);
    }
    // Initialize the database
    FILE *fp = fopen(DATABASE_FILE, "r+");
    if (fp == NULL) {
        fp = fopen(DATABASE_FILE, "w+");
        if (fp == NULL) {
            printf("kv: Failed to open database file");
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    int i, res; 
    node_t *head = (node_t*)malloc(sizeof(node_t));
    if (head == NULL) {
        fprintf(stderr, "kv: unable to allocate head");
        exit(EXIT_FAILURE);
    }
    res = initialize(head, fp);
    if (res < 0) {
        exit(EXIT_FAILURE);
    }
    // Parse arguments
    for (i=1; i<argc; i++) {
        char** arg = NULL;
        arg = malloc(sizeof(char*)*3);
        argparse(argv[i], arg);
        if (strcmp("p", arg[0]) == 0) {
            res = put(arg[1], arg[2], head);
            if (res < 0) {
                printf("kv: failed put operation: %s\n", argv[i]);
                continue;
            }
        } else if (strcmp("g", arg[0]) == 0) {
            char* val = NULL;
            res = get(arg[1], head, val);
            if (res < 0) {
                exit(EXIT_FAILURE);
            }
            if (val == NULL) {
                printf("kv: key not found");
                exit(EXIT_SUCCESS);
            }
            printf("Found value: %s", val);
        } else if (strcmp("a", arg[0]) == 0) {
            all(head);
        } else if (strcmp("d", arg[0]) == 0) {

        } else {
            printf("kv: unknown operation\n");
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        free(arg);
    }
    fclose(fp);
    exit(EXIT_SUCCESS);
}