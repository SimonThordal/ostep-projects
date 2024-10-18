#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define _GNU_SOURCE

const char* DATABASE_FILE = "database.txt";

/**
 * Enter a value into the DB with a given key.
 * Expects arguments of the form p,KEY,VALUE
 */
int put(char* _arg) {
    // Skip the first tokenization as it is "p"
    char* arg = malloc(strlen(_arg)*sizeof(char));
    arg = strncpy(arg, _arg, strlen(_arg));
    // Skip until the first comma as it is just the operation
    char *c = strtok(arg, ",");
    if (c == NULL || *c=='\0') {
        return -1;
    }
    c = strtok(NULL, ",");
    if (c == NULL || *c=='\0') {
        return -1;
    }
    // Convert the key to a string
    errno = 0;
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
    char *val = strtok(NULL, ",");
    if (val == NULL) {
        return -1;
    }
    printf("key: %ld, val: %s\n", key, val);
    return 0;
}

/**
 * List all k,v pairs in the database.
 */
int all(FILE *fp) {
    rewind(fp);
    errno = 0;
    size_t n;
    char* line = NULL;
    size_t len = 0;
    while ((n = getline(&line, &len, fp)) != -1)
    {
        if (errno != 0) {
            return -1;
        }
        printf("%s", line);
    }
    return 0;
}

/**
 * Get the value stored in a given key.
 * Expects arguments of the form g,KEY
 */
char *get(char* arg) {
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
    // Parse arguments
    int i; 
    char* arg;
    for (i=1; i<argc; i++) {
        arg = argv[i];
        if ('p' == arg[0]) {
            int res = put(arg);
            if (res < 0) {
                printf("kv: failed put operation: %s\n", arg);
                continue;
            }
        } else if ('a' == arg[0]) {
            int res = all(fp);
            if (res < 0) {
                printf("kv: failed to list database entries: %s\n", arg);
                continue;
            }
        } else {
            printf("kv: unknown operation\n");
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);
    exit(EXIT_SUCCESS);
}