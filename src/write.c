// Fixes missing %llu type warning when using Mingw/Mingw-w64
#define __USE_MINGW_ANSI_STDIO 1

#include "write.h"

int writeHeader(pnmHeader header, FILE* outputFd) {
    if(header.mode < 1 || header.mode > 7) {
        fprintf(stderr, "Error: Mode P%d not valid\n", header.mode);
        return -1;
    }

    if(header.mode != 3 && header.mode != 6) {
        fprintf(stderr, "Error: Mode P%d not supported\n", header.mode);
    }

    if(header.maxColorSize < CS430_PNM_MIN) {
        fprintf(stderr, "Error: Max color size must be at least %d\n",
            CS430_PNM_MIN);
        return -1;
    }

    if(header.maxColorSize > CS430_PNM_FULL_MAX) {
        fprintf(stderr, "Error: P%d only supports color size up to %d\n",
            header.mode, CS430_PNM_FULL_MAX);
        return -1;
    }

    fprintf(outputFd, "P%d\n", header.mode);
    if(header.comments != NULL) {
        size_t i = 0;
        while(header.comments[i] != NULL) {
            fprintf(outputFd, "# %s\n", header.comments[i++]);
        }
    }

    fprintf(outputFd, "%zu %zu\n", header.width, header.height);
    // If not P1 or P4, then write maxColorSize
    if(header.mode % 3 != 1) {
        fprintf(outputFd, "%zu\n", header.maxColorSize);
    }

    return 0;
}

int writeBody(pnmHeader header, pixel* pixels, FILE* outputFd) {
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

    // If P4 - P7, set write mode as binary.
    if(header.mode == 6) {
        // Write height * width number of pixels, which each pixel sizeof itself
        // (e.g. 3 bytes for 3 channels)
        fwrite(pixels, sizeof(*pixels), header.height * header.width, outputFd);
    }
    else if(header.mode == 3) {
        for(size_t i = 0; i < header.height; i++) {
            for(size_t j = 0; j < header.width; j++) {
                fprintf(outputFd, "%d", pixels[header.height * i + j].red);
                fprintf(outputFd, " %d", pixels[header.height * i + j].green);
                fprintf(outputFd, " %d", pixels[header.height * i + j].blue);

                // If not the last item, write a following tab.
                if(j < header.width - 1) {
                    fprintf(outputFd, "\t");
                }
            }

            fprintf(outputFd, "\n");
        }
    }
    else {
        fprintf(stderr, "Error: Mode P%d not supported\n", header.mode);
        return -1;
    }

    return 0;
}
