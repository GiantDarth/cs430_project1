// Fixes missing %llu type warning when using Mingw/Mingw-w64
#define __USE_MINGW_ANSI_STDIO 1

#include <inttypes.h>

#include "pnm.h"

int writeHeader(int mode, char** comments, size_t width, size_t height,
        size_t maxColorSize, FILE* outputFd) {
    if(mode < 1 || mode > 7) {
        fprintf(stderr, "Error: Mode P%d not valid\n", mode);
        return -1;
    }

    if(maxColorSize < CS430_PNM_MIN) {
        fprintf(stderr, "Error: Max color size must be at least %d\n",
            CS430_PNM_MIN);
        return -1;
    }

    switch(mode % 3) {
        // P1 or P4
        case(1):
            if(maxColorSize > CS430_PNM_BITMAP_MAX) {
                fprintf(stderr, "Error: P%d only supports color size up to %d\n",
                    mode, CS430_PNM_BITMAP_MAX);
                return -1;
            }

            break;
        // P2 or P5
        case(2):
            if(maxColorSize > CS430_PNM_GREY_MAX) {
                fprintf(stderr, "Error: P%d only supports color size up to %d\n",
                    mode, CS430_PNM_GREY_MAX);
                return -1;
            }

            break;
        // P3 or P6
        case(0):
            if(maxColorSize > CS430_PNM_FULL_MAX) {
                fprintf(stderr, "Error: P%d only supports color size up to %d\n",
                    mode, CS430_PNM_FULL_MAX);
                return -1;
            }

            break;
    }

    fprintf(outputFd, "P%d\n", mode);
    size_t i = 0;
    while(comments[i] != NULL) {
        fprintf(outputFd, "# %s\n", comments[i++]);
    }

    fprintf(outputFd, "%llu %llu\n", width, height);
    // If not P1 or P4, then write maxColorSize
    if(mode % 3 != 1) {
        fprintf(outputFd, "%llu\n", maxColorSize);
    }

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
