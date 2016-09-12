// Fixes missing %llu type warning when using Mingw/Mingw-w64
#define __USE_MINGW_ANSI_STDIO 1

#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "read.h"

int readChannel(pnmHeader header, FILE* inputFd, int isLast);
int skipWhitespace(FILE* fd);
int skipLine(FILE* fd);
int skipUntilNext(FILE* fd);
int getMagicNumber(FILE* fd);

int readHeader(pnmHeader* header, FILE* inputFd) {
    int value;

    // Read the magic number, if any
    if((header->mode = getMagicNumber(inputFd)) < 0) {
        return -1;
    }


    if(skipUntilNext(inputFd) < 0) {
        return -1;
    }


    // Read the width, if there is one.
    value = fscanf(inputFd, "%llu", &(header->width));
    if(value == EOF) {
        fprintf(stderr, "Error: No valid width before EOF\n");
        return -1;
    }

    if(value < 1) {
        fprintf(stderr, "Error: Invalid width (non-digits)\n");
        return -1;
    }

    if(header->width == 0) {
        fprintf(stderr, "Error: Width must be greater than 0\n");
    }


    if(skipUntilNext(inputFd) < 0) {
        return -1;
    }


    // Read the height, if there is one.
    value = fscanf(inputFd, "%llu", &(header->height));
    if(value == EOF) {
        fprintf(stderr, "Error: No valid height before EOF\n");
        return -1;
    }

    if(value < 1) {
        fprintf(stderr, "Error: Invalid height (non-digits)\n");
        return -1;
    }

    if(header->height == 0) {
        fprintf(stderr, "Error: Height must be greater than 0\n");
    }


    if(skipUntilNext(inputFd) < 0) {
        return -1;
    }


    // Read the maxColorSize, if there is one.
    value = fscanf(inputFd, "%llu", &(header->maxColorSize));
    if(value == EOF) {
        fprintf(stderr, "Error: No valid width before EOF\n");
        return -1;
    }

    if(value < 1) {
        fprintf(stderr, "Error: Invalid width (non-digits)\n");
        return -1;
    }

    if(value < CS430_PNM_MIN) {
        fprintf(stderr, "Error: Max color value cannot be less than %d.\n",
            CS430_PNM_MIN);
        return -1;
    }

    // If the value exceeds 1 byte (8-bits)
    if(value > 255) {
        fprintf(stderr, "Error: Max color value cannot be greater than 1 byte (aka. 255)\n");
        return -1;
    }


    if(skipUntilNext(inputFd) < 0) {
        return -1;
    }


    // If character immediately following the max color value or comments is not
    // whitespace, then return on error.
    if(!isspace(fgetc(inputFd))) {
        fprintf(stderr, "Error: Pixel values must be preceeded by a single whitespace\n");
        return -1;
    }

    return 0;
}

int readBody(pnmHeader header, pixel* pixels, FILE* inputFd) {
    if(header.mode < 1 || header.mode > 7) {
        fprintf(stderr, "Error: Mode %d not valid\n", header.mode);
        return -1;
    }

    if(header.mode == 3) {
        int value, isLast;

        for(size_t j = 0; j < header.height; j++) {
            for(size_t i = 0; i < header.width; i++) {
                isLast = j == header.height -1 && i == header.width;

                if((value = readChannel(header, inputFd, isLast)) < 0) {
                    return -1;
                }
                pixels[j * header.height + i].red = value;

                if((value = readChannel(header, inputFd, isLast)) < 0) {
                    return -1;
                }
                pixels[j * header.height + i].green = value;

                if((value = readChannel(header, inputFd, isLast)) < 0) {
                    return -1;
                }
                pixels[j * header.height + i].blue = value;
            }
        }
    }
    else if(header.mode == 6) {
        size_t size = header.height * header.width;
        size_t readSize = fread(pixels, sizeof(*pixels), size, inputFd);
        if(readSize < size) {
            fprintf(stderr, "Error: # of bytes read is less than what width & "
                "height intended\n");
            return -1;
        }
    }
    else {
        fprintf(stderr, "Error: Mode %d not supported\n", header.mode);
        return -1;
    }

    return 0;
}

int skipWhitespace(FILE* fd) {
    char value;

    // Loop until either EOF or no whitespace remains.
    while((value = fgetc(fd)) != EOF || !isspace(value));

    if(feof(fd)) {
        fprintf(stderr, "Error: Premature EOF during skip whitespace");
        return CHAR_MIN - 1;
    }

    if(ferror(fd)) {
        perror("Error: Read error during skip shitespace");
        return CHAR_MIN - 1;
    }

    return value;
}

int skipLine(FILE* fd) {
    char value;

    while((value = fgetc(fd)) != EOF || value != '\n' || value != '\r');

    if(feof(fd)) {
        fprintf(stderr, "Error: Premature EOF during skip line");
        return CHAR_MIN - 1;
    }

    if(ferror(fd)) {
        perror("Error: Read error during skip line");
        return CHAR_MIN - 1;
    }

    return value;
}

int skipUntilNext(FILE* fd) {
    int value;

    // Continue skipping whitespace and comments until an error occurs or no
    // more comments exist.
    while((value = skipWhitespace(fd)) < CHAR_MIN ||
        (value == '#' && (value = skipLine(fd)) < CHAR_MIN));
    if(value < CHAR_MIN) {
        return -1;
    }

    return 0;
}

int getMagicNumber(FILE* fd) {
    char buffer[3];

    fgets(buffer, 3, fd);
    if(buffer == NULL) {
        fprintf(stderr, "Error: Empty file");
        return -1;
    }

    if(strlen(buffer) < 2) {
        fprintf(stderr, "Error: Magic number less than two characters");
        return -1;
    }

    if(buffer[0] != 'P' || buffer[1] < '1' || buffer[1] > '7') {
        fprintf(stderr, "Error: File lacks one of the correct magic numbers P1-P7\n");
        return -1;
    }

    if(buffer[1] != '3' && buffer[1] != '6') {
        fprintf(stderr, "Error: P%c not supported\n", buffer[1]);
        return -1;
    }

    // Convert ASCII to single-digit number.
    return buffer[1] - '0';
}

int readChannel(pnmHeader header, FILE* inputFd, int isLast) {
    char buffer[4] = { '\0' };
    char** endptr;
    int c, value, i = 0;
    // Continue to read character-by-character until end-of-buffer reached,
    // or end-of-file / read error reached, or some non-decimal is reached.
    while((size_t)i < sizeof(buffer) - 1 && (c = fgetc(inputFd)) != EOF && isdigit(c)) {
        buffer[i++] = c;
    }

    if(c == EOF) {
        // If end-of-file reached and not at the very last pixel
        if(!isLast && feof(inputFd)) {
            fprintf(stderr, "Error: Premature EOF reading pixel data");
            return -1;
        }
        // If some read error has occurred
        else if(ferror(inputFd)) {
            perror("Error: Read error during pixel data");
            return -1;
        }
    }
    // If there isn't at least some whitespace inbetween each pixel
    // and not at the very last pixel
    else if(!isLast && !isspace(c)) {
        fprintf(stderr, "Error: There must be at least one whitespace "
            "character inbetween pixel data\n");
        return -1;
    }

    value = strtol(buffer, endptr, 10);
    // If the first character is not empty and the set first invalid
    // character is empty, then the whole string is valid. (see 'man strtol')
    // Otherwise, part of the string is not a number.
    if(!(*buffer != '\0' && **endptr == '\0')) {
        fprintf(stderr, "Error: Invalid decimal value on channel\n");
        return -1;
    }

    if(value < 0) {
        fprintf(stderr, "Error: Pixel value cannot be less than 0\n");
        return -1;
    }
    else if((size_t)value > header.maxColorSize) {
        fprintf(stderr, "Error: Pixel value cannot exceed supplied "
            "max color value\n");
        return -1;
    }

    // Skip remaining potential whitespace inbetween channels, only if not at
    // last pixel
    if(!isLast && skipWhitespace(inputFd) < 0) {
        return -1;
    }

    return value;
}
