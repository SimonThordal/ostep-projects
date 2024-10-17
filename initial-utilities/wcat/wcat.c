#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE* fp;
    char buf[20];
    // If no file is provided this is a noop
    if (argc == 1) {
        exit(0);
    }
    // Iterate over argument
    for (int i = 1; i < argc; i++) {
        fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wcat: cannot open file\n");
            exit(1);
        }
        while(fgets(buf, sizeof(buf), fp) != NULL) {
            printf("%s", buf);
        }
        fclose(fp);
    }
    exit(0);
}