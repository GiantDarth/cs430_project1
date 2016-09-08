#include <stdio.h>
#include <stdlib.h>

#include "pnm.h"

struct args {
    // Some integer 1-6
    int mode;
    const char* inputPath;
    const char* outputPath;
};

int main(int argc, const char* argv[]) {
    if(argc != 4) {
        perror("usage: /path/to/cmd mode /path/to/inputFile /path/to/outputFile");
        exit(EXIT_FAILURE);
    }

    struct args args;
    char** endptr;
    // Convert arg1 to integer such as "3"
    args.mode = strtol(argv[1], endptr, 10);
    // If the first character is not empty and the set first invalid
    // character is empty, then the whole string is valid. (see 'man strtol')
    // Otherwise, part of the string is not a number.
    if(!(*(argv[1]) != '\0' && **endptr == '\0')) {
        fprintf(stderr, "Error: Invalid mode\n");
        exit(EXIT_FAILURE);
    }
    else if(args.mode < 1 || args.mode > 7) {
        fprintf(stderr, "Error: P%d does not exist\n", args.mode);
        exit(EXIT_FAILURE);
    }
    else if(args.mode == 7) {
        fprintf(stderr, "Error: P7 is currently not supported\n");
        exit(EXIT_FAILURE);
    }

    args.inputPath = argv[2];
    args.outputPath = argv[3];

    FILE* inputFd;
    FILE* outputFd;

    if((inputFd = fopen(args.inputPath, "r")) == NULL) {
        perror("Error: opening input file\n");
        exit(EXIT_FAILURE);
    }

    if((outputFd = fopen(args.outputPath, "w")) == NULL) {
        perror("Error: opening output file\n");
        exit(EXIT_FAILURE);
    }

    // Read the file, get format
    // Write the file as new format (from parameter)

    return EXIT_SUCCESS;
}
