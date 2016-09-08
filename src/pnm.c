// Fixes missing %llu type warning when using Mingw/Mingw-w64
#define __USE_MINGW_ANSI_STDIO 1

#include <inttypes.h>

#include "pnm.h"

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

int writeBody(pnmHeader header, char* buffer, FILE* outputFd) {
    if(header.mode < 1 || header.mode > 7) {
        fprintf(stderr, "Error: Mode P%d not valid\n", header.mode);
        return -1;
    }

    if(header.maxColorSize < CS430_PNM_MIN) {
        fprintf(stderr, "Error: Max color size must be at least %d\n",
            CS430_PNM_MIN);
        return -1;
    }

    size_t size = 1;
    int binFlag = 0;

    // If P4 - P7, set write mode as binary.
    if(header.mode >= 4) {
        binFlag = 1;
    }

    // P5 or P6 and color size is greater than 255, then use 2 bytes
    if(binFlag && (header.mode % 3 == 2 || header.mode % 3 == 0)
            && header.maxColorSize > 255) {
        size = 2;
    }

    if(binFlag) {
        for(size_t i = 0; i < header.height; i++) {
            for(size_t j = 0; j < header.width; j++) {
                buffer[header.height * j + i];
            }
        }
    }
    else {
        for(size_t i = 0; i < header.height; i++) {
            for(size_t j = 0; j < header.width; j++) {

            }
        }
    }

    return 0;
}
