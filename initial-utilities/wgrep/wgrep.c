#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    FILE *fp = stdin;
    char *line;
    size_t len = 0;
    size_t nread;

    if(argc < 2) {
        printf("wgrep: searchterm [file ...]\n");
        exit(EXIT_FAILURE);
    }
    if(argc==2) {
        if (isatty(fileno(stdin))) {
            exit(EXIT_SUCCESS);
        }
        fp = stdin;
    } else {
        if ((fp = fopen(argv[2], "r")) == NULL) {
            printf("wgrep: cannot open file\n");
            exit(EXIT_FAILURE);
        }
    }
    while ((nread=getline(&line, &len, fp)) != -1) {
        if (strstr(line, argv[1]) != NULL) {
            printf("%s", line);
        }
    }
    free(line);
    fclose(fp);
    exit(EXIT_SUCCESS);
}