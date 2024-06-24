#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

typedef struct {
    uint8_t gray;
    uint16_t count;
} RLEPacket;

void skipComments(FILE *file) {
    int ch;
    char line[1024];
    while ((ch = fgetc(file)) != EOF && isspace(ch));
    if (ch == '#') {
        fgets(line, sizeof(line), file);
        skipComments(file);
    } else {
        fseek(file, -1, SEEK_CUR);
    }
}

void compressPBM(const char *inputFile, const char *outputFile) {
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
    int width, height;
    
    fscanf(in, "%2s", format);
    skipComments(in);
    fscanf(in, "%d %d", &width, &height);
    skipComments(in);

    fprintf(out, "%s\n%d %d\n", format, width, height);

    int binary = format[1] == '4';
    uint8_t *imageData = (uint8_t *)malloc(width * height * sizeof(uint8_t));
    if (!imageData) {
        perror("Memory allocation failed");
        fclose(in);
        fclose(out);
        exit(EXIT_FAILURE);
    }

    if (binary) {
        fread(imageData, sizeof(uint8_t), width * height / 8, in);
    } else {
        for (int i = 0; i < width * height; i++) {
            fscanf(in, "%hhu", &imageData[i]);
        }
    }

    for (int i = 0; i < width * height; ) {
        uint8_t bit = imageData[i];
        uint16_t count = 0;

        while (i < width * height && imageData[i] == bit && count < 65535) {
            count++;
            i++;
        }

        RLEPacket packet = { bit, count };
        fwrite(&packet, sizeof(RLEPacket), 1, out);
    }

    free(imageData);
    fclose(in);
    fclose(out);
}

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
    
    fscanf(in, "%2s", format);
    skipComments(in);
    fscanf(in, "%d %d", &width, &height);
    skipComments(in);
    fscanf(in, "%d", &maxVal);
    skipComments(in);
    fprintf(out, "%s\n%d %d\n%d\n", format, width, height, maxVal);

    int binary = format[1] == '5';
    uint8_t *imageData = (uint8_t *)malloc(width * height * sizeof(uint8_t));
    if (!imageData) {
        perror("Memory allocation failed");
        fclose(in);
        fclose(out);
        exit(EXIT_FAILURE);
    }

    if (binary) {
        fread(imageData, sizeof(uint8_t), width * height, in);
    } else {
        for (int i = 0; i < width * height; i++) {
            fscanf(in, "%hhu", &imageData[i]);
        }
    }

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

void compressPPM(const char *inputFile, const char *outputFile) {
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
    fscanf(in, "%2s", format);
    skipComments(in);
    fscanf(in, "%d %d", &width, &height);
    skipComments(in);
    fscanf(in, "%d", &maxVal);
    skipComments(in);
    fprintf(out, "%s\n%d %d\n%d\n", format, width, height, maxVal);

    int binary = format[1] == '6';
    uint8_t *imageData = (uint8_t *)malloc(3 * width * height * sizeof(uint8_t));
    if (!imageData) {
        perror("Memory allocation failed");
        fclose(in);
        fclose(out);
        exit(EXIT_FAILURE);
    }

    if (binary) {
        fread(imageData, sizeof(uint8_t), 3 * width * height, in);
    } else {
        for (int i = 0; i < 3 * width * height; i++) {
            fscanf(in, "%hhu", &imageData[i]);
        }
    }
    for (int i = 0; i < 3 * width * height; ) {
        uint8_t color = imageData[i];
        uint16_t count = 0;

        while (i < 3 * width * height && imageData[i] == color && count < 65535) {
            count++;
            i++;
        }

        RLEPacket packet = { color, count };
        fwrite(&packet, sizeof(RLEPacket), 1, out);
    }

    free(imageData);
    fclose(in);
    fclose(out);
}

void compressNetpbm(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "rb");
    if (!in) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    char format[3];
    fscanf(in, "%2s", format);
    fclose(in);

    if (strcmp(format, "P1") == 0 || strcmp(format, "P4") == 0) {
        compressPBM(inputFile, outputFile);
    } else if (strcmp(format, "P2") == 0 || strcmp(format, "P5") == 0) {
        compressPGM(inputFile, outputFile);
    } else if (strcmp(format, "P3") == 0 || strcmp(format, "P6") == 0) {
        compressPPM(inputFile, outputFile);
    } else {
        fprintf(stderr, "Unsupported Netpbm format: %s\n", format);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.netpbm> <output.rle>\n", argv[0]);
        return EXIT_FAILURE;
    }

    compressNetpbm(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
