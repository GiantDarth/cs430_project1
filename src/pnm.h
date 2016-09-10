#ifndef CS430_PNM_H
#define CS430_PNM_H

#include <stdio.h>
#include <stdlib.h>

#define CS430_PNM_MIN 1
#define CS430_PNM_BITMAP_MAX 255
#define CS430_PNM_GREY_MAX 65535
#define CS430_PNM_FULL_MAX 65535

typedef struct pnmHeader {
    int mode;
    char** comments;
    size_t width;
    size_t height;
    size_t maxColorSize;
} pnmHeader;

typedef struct pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} pixel;

int readHeader(pnmHeader& header, FILE* inputFd);

int writeHeader(pnmHeader header, FILE* outputFd);
int writeBody(pnmHeader header, char* buffer, FILE* outputFd);

#endif // CS430_PNM_H
