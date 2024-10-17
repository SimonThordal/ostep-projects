#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    char c;
    int n;
    int i, j;
    if (argc < 2) {
        printf("wunzip: file1 [file2 ...]\n");
        exit(EXIT_FAILURE);
    }
    for (j=1;j < argc; j++) {
        FILE *fp = fopen(argv[j], "r");
        if (fp == NULL) {
            printf("wunzip: could not open file\n");
            exit(EXIT_FAILURE);
        }
        while (fread(&n, sizeof(int), 1, fp) > 0) {
            c = getc(fp);
            for (i=0; i<n; i++) {
                putchar(c);
            }
        }
        fclose(fp);
    }
    exit(EXIT_SUCCESS);

}