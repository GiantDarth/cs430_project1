#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pnm.h"
#include "read.h"
#include "write.h"

struct args {
    // Some integer 1-6
    int mode;
    const char* inputPath;
    const char* outputPath;
};

int main(int argc, const char* argv[]) {
    if(argc != 4) {
        fprintf(stderr, "usage: ppmrw [3|6] /path/to/inputFile /path/to/outputFile\n");
        exit(EXIT_FAILURE);
    }

    struct args args;
    char* endptr;
    // Convert arg1 to integer such as "3"
    args.mode = strtol(argv[1], &endptr, 10);
    // If the first character is not empty and the set first invalid
    // character is empty, then the whole string is valid. (see 'man strtol')
    // Otherwise, part of the string is not a number.
    if(!(*(argv[1]) != '\0' && *endptr == '\0')) {
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

    pnmHeader header;
    pixel* pixels;

    // Read the file, get format
    if(readHeader(&header, inputFd) < 0) {
        exit(EXIT_FAILURE);
    }

    if((pixels = (pixel*)malloc(sizeof(*pixels) * header.width * header.height)) == NULL) {
        perror("Error: Memory allocation error on pixels\n");
        exit(EXIT_FAILURE);
    }
    if(readBody(header, pixels, inputFd) < 0) {
        exit(EXIT_FAILURE);
    }

    // Write the file as new format (from parameter)
    header.mode = args.mode;
    char comments[][80] = {
        "Created with ppmrw (Christopher Philabaum <cp723@nau.edu>)"
    };

    header.comments = malloc(sizeof(comments) + 1);
    for(size_t i = 0; i < sizeof(comments) ; i++) {
        header.comments[i] = malloc(sizeof(*comments));
        strcpy(header.comments[i], comments[i]);
    }
    header.comments[sizeof(comments)] = malloc(sizeof(*comments));
    header.comments[sizeof(comments)] = NULL;

    if(writeHeader(header, outputFd) < 0) {
        exit(EXIT_FAILURE);
    }
    if(writeBody(header, pixels, outputFd) < 0) {
        exit(EXIT_FAILURE);
    }

    // No need to free pixels as memory will be released once program ends

    return EXIT_SUCCESS;
}
