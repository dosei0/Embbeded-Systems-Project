#include "ffconf.h"
#include "ff.h"
#include "lpc_types.h"

#include "serial.h"
#include "commands.h"
#include "files.h"

#include <stdio.h>
#include <string.h>


void listFiles(char *path, char files[100][17]){
	FRESULT res;
    DIR dir;
    FILINFO fno;
	res = f_opendir(&dir, path);                       /* Open the directory */
    int i = 0;
    if (res == FR_OK) {
		write_usb_serial_blocking("CLRFILS\n\r",9);
		for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
				//Do nothing
            } else {                                       /* It is a file. */
				char out[100];
                sprintf(out, "F: %s\n\r", fno.fname);
				write_usb_serial_blocking(out,strlen(out));
                strcpy(files[i], fno.fname);
                i++;
            }
        }
        f_closedir(&dir);
		write_usb_serial_blocking("ENDFILS\n\r",9);
    }
}


__uint32_t read32(__uint8_t *b){
    return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
}
__uint16_t read16(__uint8_t *b){
    return b[0] | (b[1] << 8);
}
void write32(__uint8_t *out, __uint32_t n){
    out[0] = (n & 0xFF);
    out[1] = (n & 0xFF00) >> 8;
    out[2] = (n & 0xFF0000) >> 16;
    out[3] = (n & 0xFF000000) >> 24;
}
void writestr(__uint8_t *out, char* s, int len){
    int i;
    for(i = 0; i < len; i++){
        out[i] = s[i];
    }
}

void writeWavHeader(__uint8_t *out, __uint32_t dataSize, __uint8_t numChannels, __uint16_t sampleRate, __uint8_t bitsPerSample){
    writestr(&out[0], "RIFF", 4);           // ChunkID
    write32(&out[4], dataSize + 36);        // ChunkSize
    writestr(&out[8], "WAVEfmt ", 8);       // Format & Subchunk1ID
    write32(&out[16], 16);                  // Subchunk1Size
    out[20] = 0x01;                         // AudioFormat
    out[21] = 0x00;                         //
    out[22] = numChannels;                  // Num channels (Stereo/mono)
    out[23] = 0x00;                         //
    write32(&out[24], sampleRate);          // SampleRate
    int blockAlign = numChannels*bitsPerSample/8;
    write32(&out[28], (int)(sampleRate*blockAlign)); // ByteRate
    out[32] = blockAlign;                   // Block Align
    out[33] = 0x00;                         //
    out[34] = bitsPerSample;                // BitsPerSample
    out[35] = 0x00;                         //
    writestr(&out[36], "data", 4);          // Subchunk2ID
    write32(&out[40], dataSize);            // Subchunk2Size
}

void readWavHeader(char* head, WAVHEADER* meta){
    meta->datasize      = read32(&head[40]);
    meta->numChannels   = read16(&head[22]);
    meta->sampleRate    = read32(&head[24]);
    meta->bitsPerSample = read16(&head[34]);
    meta->blockAlign    = read16(&head[32]);
    meta->duration      = (meta->datasize / (meta->bitsPerSample * meta->numChannels))*8 / meta->sampleRate;
}

void copyWAV(COMMAND cmd, char* buffer, int reverse){
    FIL fsrc, fdst;
    FRESULT fr;
    UINT br, bw;

    // Open source file
    fr = f_open(&fsrc, cmd.infile, FA_READ);
    if (fr != FR_OK){
        char str[32];
        sprintf(str, "Failed to open infile:%7d\n\r", fr);
        write_usb_serial_blocking(str,31);
        return;
    }

    // Create destination file
    fr = f_open(&fdst, "reverse.wav", FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK){
        char str[34];
        sprintf(str, "Failed to create outfile:%7d\n\r", fr);
        write_usb_serial_blocking(str,33);
        f_close(&fsrc);
        return;
    }

    // Read the metadata
    char header[44];
    fr = f_read(&fsrc, header, 44, &br);
    if (fr != FR_OK){
        char str[32];
        sprintf(str, "Failed to read infile:%7d\n\r", fr);
        write_usb_serial_blocking(str,31);
        return;
    }
    WAVHEADER meta;
    readWavHeader(header, &meta);

    // Write the same metadata to the output file
    f_write(&fdst, header, 44, &bw);

    write_usb_serial_blocking("Copy started\n\r",14);

    // Copy source to destination, block by block
    int i;
    for (i=44; i < meta.datasize; i += meta.blockAlign) {
        fr = f_read(&fsrc, buffer, sizeof buffer, &br); // Read a chunk of source file
        if (fr || br == 0) break;                       // error or eof
        if (reverse) {                                  // If copying in reverse, manually seek
            fr = f_lseek(&fdst, (meta.datasize + 44) - i);     // the write location each block
            if (fr || br == 0) break;
        }
        fr = f_write(&fdst, buffer, br, &bw);           // Write it to the destination file
        if (fr || bw < br) break;                       // error or disk full
        // Replace with a better progress thing later, maybe on the LCD?
        if(i % 10000 == 0){
            char out[20];
            sprintf(out, "%8d/%8d\n\r", i, meta.datasize);
            write_usb_serial_blocking(out,19);
        }
    }

    write_usb_serial_blocking("Copy over\n\r",11);

    if(fr != FR_OK){
        // File access failed
        char str[30];
        sprintf(str, "Failed to copy file:%7d\n\r", fr);
        write_usb_serial_blocking(str,29);
        // Don't return here since we need to close the files either way
    }

    /* Close open files */
    f_close(&fsrc);
    f_close(&fdst);
}


void copy(COMMAND cmd, char* buffer){
    FIL fsrc, fdst;
    FRESULT fr;
    UINT br, bw;

    // Open source file on the drive 1
    fr = f_open(&fsrc, cmd.infile, FA_READ);
    if (fr != FR_OK){
        char str[32];
        sprintf(str, "Failed to read infile:%7d\n\r", fr);
        write_usb_serial_blocking(str,31);
        return;
    }

    // Create destination file on the drive 0
    fr = f_open(&fdst, cmd.outfile, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK){
        char str[34];
        sprintf(str, "Failed to create outfile:%7d\n\r", fr);
        write_usb_serial_blocking(str,33);
        f_close(&fsrc);
        return;
    }

    // Copy source to destination
    for (;;) {
        fr = f_read(&fsrc, buffer, sizeof buffer, &br); // Read a chunk of source file
        if (fr || br == 0) break;                       // error or eof
        fr = f_write(&fdst, buffer, br, &bw);           // Write it to the destination file
        if (fr || bw < br) break;                       // error or disk full
    }
    if(fr != FR_OK){
        // File access failed
        char str[30];
        sprintf(str, "Failed to copy file:%7d\n\r", fr);
        write_usb_serial_blocking(str,29);
        // Don't return here since we need to close the files either way
    }

    // Close open files
    f_close(&fsrc);
    f_close(&fdst);
}

void getVolumeGraph(char* filename, /*uint8_t* vols,*/ uint16_t windowPkts, uint16_t numSamples){
    FIL fsrc;
    FRESULT fr;
    UINT br;

    fr = f_open(&fsrc, filename, FA_READ);
    if (fr != FR_OK){
        char str[32];
        sprintf(str, "Failed to read infile:%7d\n\r", fr);
        write_usb_serial_blocking(str,31);
        return;
    }

    // Read the metadata
    char header[44];
    fr = f_read(&fsrc, header, 44, &br);
    if (fr != FR_OK){
        char str[32];
        sprintf(str, "Failed to open infile:%7d\n\r", fr);
        write_usb_serial_blocking(str,31);
        return;
    }

    WAVHEADER meta;
    readWavHeader(header, &meta);

    // Allocate buffer
    int8_t buffer[windowPkts*meta.blockAlign];

    write_usb_serial_blocking("CLRVALS\n\r",9);

    int i,j,v = 0;
    int pktStep = (((meta.datasize/meta.blockAlign)-windowPkts) / numSamples) * meta.blockAlign;
    float lastPr = 0;
    for (i=44; i < meta.datasize+44; i += pktStep) {
        fr = f_lseek(&fsrc, i);
        if (fr || br == 0) break;
        fr = f_read(&fsrc, buffer, sizeof buffer, &br); // Read a chunk of source file
        if (fr || br == 0) break;                       // error or eof
        // Calculate a rough volume estimate from this buffer
        int avg = 0;
        for(j=1; j < sizeof buffer; j += meta.blockAlign){
            if(buffer[j] < 0){
                avg -= buffer[j];
            } else {
                avg += buffer[j];
            }
        }
        avg /= numSamples;
        char out[14];
        sprintf(out, "V: %8d\n\r", avg);
		write_usb_serial_blocking(out,13);
    }

    write_usb_serial_blocking("ENDVALS\n\r",9);

    if(fr != FR_OK){
        // File access failed
        char str[30];
        sprintf(str, "Failed to read file:%7d\n\r", fr);
        write_usb_serial_blocking(str,29);
        // Don't return here since we need to close the files either way
    }
}
