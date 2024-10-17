#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int main(int argc, char *argv[]) {
    FILE *fin;
    unsigned int c;
    int prev = -1;
    int i;
    unsigned int count = 0;
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(EXIT_FAILURE);
    }
    for (i=1; i < argc; i++) {
        fin = fopen(argv[i], "r");
        if (fin == NULL) {
            printf("wzip: error opening file.");
            exit(EXIT_FAILURE);
        }
        while ((c = getc(fin)) != EOF) {
            // If it is not the first character and the curret character does not match the previous character,
            // print it and reset the counter
            if (prev != -1 && c != prev) {
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&prev, sizeof(char), 1, stdout);
                count = 0;
            }
            // Increment the counter
            count++;
            // Update the previously seen character
            prev = c;
        }
        fclose(fin);
    }
    if (count > 0) {
        fwrite(&count, sizeof(int), 1, stdout);
        fwrite(&prev, sizeof(char), 1, stdout);
    }
    exit(EXIT_SUCCESS);
}