// Fixes missing %llu type warning when using Mingw/Mingw-w64
#define __USE_MINGW_ANSI_STDIO 1

#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#include "pnm.h"

int readChannel(pnmHeader header, FILE* inputFd, int isLast);
int skipWhitespace(FILE* fd);
int skipLine(FILE* fd);
int getMagicNumber(FILE* fd);

int skipWhitespace(FILE* fd) {
    char value;

    // Loop until either EOF or no whitespace remains.
    while((value = fgetc(fd)) != EOF || !isspace(value));

    if(feof(fd)) {
        fprintf(stderr, "Error: File missing width");
        return CHAR_MIN - 1;
    }

    if(ferror(fd)) {
        perror("Error: Read error");
        return CHAR_MIN - 1;
    }

    return value;
}

int skipLine(FILE* fd) {
    char value;

    while((value = fgetc(fd)) != EOF || value != '\n' || value != '\r');

    if(feof(fd)) {
        fprintf(stderr, "Error: File missing width");
        return CHAR_MIN - 1;
    }

    if(ferror(fd)) {
        perror("Error: Read error");
        return CHAR_MIN - 1;
    }

    return value;
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

int readHeader(pnmHeader* header, FILE* inputFd) {
    char buffer[80];
    int value;

    // Read the magic number, if any
    if((header->mode = getMagicNumber(inputFd)) < 0) {
        return -1;
    }

    if((value = skipWhitespace(inputFd)) < CHAR_MIN) {
        return -1;
    }

    // If the remaining line starts with '#', skip until newline.
    if(value == '#') {
        if(skipLine(inputFd) < CHAR_MIN) {
            return -1;
        }
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

    if((value = skipWhitespace(inputFd)) < CHAR_MIN) {
        return -1;
    }

    if(value == '#') {
        if(skipLine(inputFd) < CHAR_MIN) {
            return -1;
        }
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

    if((value = skipWhitespace(inputFd)) < CHAR_MIN) {
        return -1;
    }

    if(value == '#') {
        if(skipLine(inputFd) < CHAR_MIN) {
            return -1;
        }
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

    if(value == '#') {
        if(skipLine(inputFd) < CHAR_MIN) {
            return -1;
        }
    }

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

int readChannel(pnmHeader header, FILE* inputFd, int isLast) {
    long long value;

    fscanf(inputFd, "%lld", &value);
    if(value < 0) {
        fprintf(stderr, "Error: Pixel value cannot be less than 0\n");
        return -1;
    }
    else if((size_t)value > header.maxColorSize) {
        fprintf(stderr, "Error: Pixel value cannot exceed supplied "
            "max color value\n");
    }

    value = fgetc(inputFd);
    if(value == EOF) {
        // If end-of-file reached and not at the very last pixel
        if(feof(inputFd) && !isLast) {
            fprintf(stderr, "Error: EOF reached before intended width"
                " & height # of pixels read.\n");
            return -1;
        }
        // If some read error has occurred
        else if(ferror(inputFd)) {
            fprintf(stderr, "Error: Read error on raster data\n");
            return -1;
        }
    }
    // If there isn't at least some whitespace inbetween each pixel
    // and not at the very last pixel
    else if(!isspace(value) && !isLast) {
        fprintf(stderr, "Error: There must be at least one whitespace"
            "character inbetween pixel data\n");
        return -1;
    }

    return value;
}

int writeHeader(pnmHeader header, FILE* outputFd) {
    if(header.mode < 1 || header.mode > 7) {
        fprintf(stderr, "Error: Mode P%d not valid\n", header.mode);
        return -1;
    }

    if(header.maxColorSize < CS430_PNM_MIN) {
        fprintf(stderr, "Error: Max color size must be at least %d\n",
            CS430_PNM_MIN);
        return -1;
    }

    switch(header.mode % 3) {
        // P1 or P4
        case(1):
            if(header.maxColorSize > CS430_PNM_BITMAP_MAX) {
                fprintf(stderr, "Error: P%d only supports color size up to %d\n",
                    header.mode, CS430_PNM_BITMAP_MAX);
                return -1;
            }

            break;
        // P2 or P5
        case(2):
            if(header.maxColorSize > CS430_PNM_GREY_MAX) {
                fprintf(stderr, "Error: P%d only supports color size up to %d\n",
                    header.mode, CS430_PNM_GREY_MAX);
                return -1;
            }

            break;
        // P3 or P6
        case(0):
            if(header.maxColorSize > CS430_PNM_FULL_MAX) {
                fprintf(stderr, "Error: P%d only supports color size up to %d\n",
                    header.mode, CS430_PNM_FULL_MAX);
                return -1;
            }

            break;
    }

    fprintf(outputFd, "P%d\n", header.mode);
    size_t i = 0;
    while(header.comments[i] != NULL) {
        fprintf(outputFd, "# %s\n", header.comments[i++]);
    }

    fprintf(outputFd, "%llu %llu\n", header.width, header.height);
    // If not P1 or P4, then write maxColorSize
    if(header.mode % 3 != 1) {
        fprintf(outputFd, "%llu\n", header.maxColorSize);
    }

    return 0;
}

int writeBody(pnmHeader header, pixel* imageBuffer, FILE* outputFd) {
    if(header.mode < 1 || header.mode > 7) {
        fprintf(stderr, "Error: Mode P%d not valid\n", header.mode);
        return -1;
    }

    if(header.maxColorSize < CS430_PNM_MIN) {
        fprintf(stderr, "Error: Max color size must be at least %d\n",
            CS430_PNM_MIN);
        return -1;
    }

    // 2 byte channels are not supported in this program
    if(header.maxColorSize > 255) {
        fprintf(stderr, "Error: 2-byte colors not supported.\n");
        return -1;
    }

    size_t channelCount = 1;
    // If P3 or P6 (e.g. Full color), then use 3 channels.
    if(header.mode % 3 == 0) {
        channelCount = 3;
    }

    // If P4 - P7, set write mode as binary.
    if(header.mode >= 4) {
        fwrite(imageBuffer, channelCount, header.height * header.width, outputFd);
    }
    else {
        for(size_t i = 0; i < header.height; i++) {
            for(size_t j = 0; j < header.width; j++) {
                fprintf(outputFd, "%d", imageBuffer[header.height * i + j]
                        .channels[0]);
                // If P3 or P6 (Full color), then write remaining 2 channels
                if(header.mode % 3 == 0) {
                    fprintf(outputFd, " %d", imageBuffer[header.height * i + j]
                            .channels[1]);
                    fprintf(outputFd, " %d", imageBuffer[header.height * i + j]
                            .channels[2]);
                }

                // If not the last item, write a following tab.
                if(j < header.width - 1) {
                    fprintf(outputFd, "\t");
                }
            }

            fprintf(outputFd, "\n");
        }
    }

    return 0;
}
