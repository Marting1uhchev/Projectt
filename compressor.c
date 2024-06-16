#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint8_t gray;
    uint16_t count;
} RLEPacket;

void compressPGM(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "rb");
    if (!in) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    FILE *out = fopen(outputFile, "wb");
    if (!out) {
        perror("Failed to open output file");
        fclose(in);
        exit(EXIT_FAILURE);
    }

    char format[3];
    int width, height, maxVal;
    
    fscanf(in, "%2s\n", format);
    fscanf(in, "%d %d\n%d\n", &width, &height, &maxVal);

    // Writing header to output file
    fprintf(out, "%s\n%d %d\n%d\n", format, width, height, maxVal);

    if (format[0] != 'P' || format[1] != '5') {
        fprintf(stderr, "Unsupported format. Only P5 (binary PGM) is supported.\n");
        fclose(in);
        fclose(out);
        exit(EXIT_FAILURE);
    }

    uint8_t *imageData = (uint8_t *)malloc(width * height * sizeof(uint8_t));
    if (!imageData) {
        perror("Memory allocation failed");
        fclose(in);
        fclose(out);
        exit(EXIT_FAILURE);
    }

    fread(imageData, sizeof(uint8_t), width * height, in);

    for (int i = 0; i < width * height; ) {
        uint8_t gray = imageData[i];
        uint16_t count = 0;

        while (i < width * height && imageData[i] == gray && count < 65535) {
            count++;
            i++;
        }

        RLEPacket packet = { gray, count };
        fwrite(&packet, sizeof(RLEPacket), 1, out);
    }

    free(imageData);
    fclose(in);
    fclose(out);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.pgm> <output.rle>\n", argv[0]);
        return EXIT_FAILURE;
    }

    compressPGM(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
