#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int same_file(FILE* fp1, FILE* fp2) {
    struct stat stat1, stat2;
    if (fstat(fileno(fp1), &stat1) < 0) return -1;
    if (fstat(fileno(fp2), &stat2) < 0) return -1;
    return (stat1.st_dev) == (stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}

int main(int argc, char *argv[]) {
    FILE *fin;
    FILE *fout;
    char *line = NULL;
    size_t n = 0;
    size_t read;
    int i;

    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(EXIT_FAILURE);
    }
    fin = stdin;
    fout = stdout;
    if (argc > 1) {
        fin = fopen(argv[1], "r");
        if (fin == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }
    if (argc == 3) {
        fout = fopen(argv[2], "w");
        if (fout == NULL) {
            fprintf(stderr, "reverse: cannot open file: '%s'\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }
    if (same_file(fin, fout)) {
        fprintf(stderr, "reverse: input and output file must differ\n");
        exit(EXIT_FAILURE);
    }
    // Go to the very end of the file
    fseek(fin, 0, SEEK_END);
    // Go backwards through the stream until we hit an EOL or the beginning
    while(ftell(fin) > 0) {
        fseek(fin, -2, SEEK_CUR);
        // If we hit an EOL, print the line and move back to the beginning of the line
        if (ftell(fin) == 0 || fgetc(fin) == '\n') {
            i = ftell(fin);
            read = getline(&line, &n, fin);
            if (read == -1) {
                printf("reverse: unable to read line\n");
                exit(EXIT_FAILURE);
            }
            fprintf(fout, "%s", line);
            fseek(fin, i, SEEK_SET);
        }
        if (ftell(fin) < 2) {
            break;
        }
    }
    fclose(fin);
    free(line);
    exit(EXIT_SUCCESS);
}
