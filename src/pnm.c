// Fixes missing %llu type warning when using Mingw/Mingw-w64
#define __USE_MINGW_ANSI_STDIO 1

#include <inttypes.h>

#include "pnm.h"

int writeHeader(int mode, char** comments, size_t width, size_t height,
        size_t maxColorSize, FILE* outputFd) {
    switch(mode % 3) {
        case(1):
            if(maxColorSize > CS43)
        case(2):
        case(0):

    }

    fprintf(outputFd, "P%d\n", mode);
    size_t i = 0;
    while(comments[i] != NULL) {
        fprintf(outputFd, "# %s\n", comments[i++]);
    }

    fprintf(outputFd, "%llu %llu\n", width, height);
    fprintf(outputFd, "%u\n", maxColorSize);

    return 0;
}

int writeBody(char* buffer, int mode, FILE* outputFd) {
    if(mode < 1 || mode > 7) {
        return -1;
    }

    size_t size;
    int binFlag = 0;

    // If P4-P7, set write mode as binary.
    if(mode >= 4) {
        binFlag = 1;
    }

    if(mode != 7) {
        switch(mode % 3) {
            // P1 or P4 = bitmap (1-bit)
            case(1):

                break;
            // P2 or P5 = greyscale (16-bit)
            case(2):
                break;
            // P3 or P6 = full color (24-bit)
            case(0):
                break;
        }
    }
}
