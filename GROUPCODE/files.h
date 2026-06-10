#define FORWARDS 0
#define REVERSE  1
#include "lpc_types.h"

#include <stdio.h>

void listFiles(char *path, char files[100][17]);
void write32(__uint8_t*, __uint32_t);
void writeWavHeader(__uint8_t *out, __uint32_t dataSize, __uint8_t numChannels, __uint16_t sampleRate, __uint8_t bitsPerSample);
void getVolumeGraph(char* filename, /*uint8_t* vols,*/ uint16_t windowPkts, uint16_t numSamples);

typedef struct {
	int datasize;
	int numChannels;
	int sampleRate;
    int bitsPerSample;
    int duration;
    int blockAlign;
} WAVHEADER;

void readWavHeader(char* head, WAVHEADER* meta);
