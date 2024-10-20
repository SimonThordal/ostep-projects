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
 * Turn a k,v string into a node struct.
 */
int parse(char* row, node_t* node) {
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
        parse(line, head);
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
 * Expects arguments of the form p,KEY,VALUE
 */
int put(char* _arg, node_t *head) {
    // Skip the first tokenization as it is "p"
    char* arg = malloc(strlen(_arg)*sizeof(char));
    arg = strncpy(arg, _arg, strlen(_arg));
    // Skip until the first comma as it is just the operation
    char *c = strtok(arg, ",");
    if (c == NULL || *c=='\0') {
        return -1;
    }
    node_t *node = (node_t*)malloc(sizeof(node_t));
    int res = parse(&arg[2], node);
    if (res < 0) {
        return -1;
    }
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
char *get(char* arg, node_t* head) {
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
    node_t *head = (node_t*)malloc(sizeof(node_t));
    if (head == NULL) {
        fprintf(stderr, "kv: unable to allocate head");
        exit(EXIT_FAILURE);
    }
    int res = initialize(head, fp);
    if (res < 0) {
        exit(EXIT_FAILURE);
    }
    // Parse arguments
    int i; 
    char* arg;
    for (i=1; i<argc; i++) {
        arg = argv[i];
        if ('p' == arg[0]) {
            int res = put(arg, head);
            if (res < 0) {
                printf("kv: failed put operation: %s\n", arg);
                continue;
            }
        } else if ('a' == arg[0]) {
            all(head);
        } else {
            printf("kv: unknown operation\n");
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);
    exit(EXIT_SUCCESS);
}