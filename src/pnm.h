#ifndef CS430_PNM_H
#define CS430_PNM_H

#include <stdio.h>
#include <stdlib.h>

#define CS430_PNM_MIN 1
#define CS430_PNM_BITMAP_MAX 255
#define CS430_PNM_GREY_MAX 65535
#define CS430_PNM_FULL_MAX 65535

struct pnm {
    int mode;
    char** comments;
    size_t width;
    size_t height;
};

int writeHeader(int mode, char** comments, size_t width, size_t height,
    unsigned char maxColor, FILE* outputFd);
int writeBody(char* buffer, int mode, FILE* outputFd);

#endif // CS430_PNM_H
