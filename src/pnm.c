// Fixes missing %llu type warning when using Mingw/Mingw-w64
#define __USE_MINGW_ANSI_STDIO 1

#include <inttypes.h>

#include "pnm.h"

int readHeader(pnmHeader* header, FILE* outputFd) {

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
