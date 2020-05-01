#include <stdio.h>
#include "cryptomath.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define BLOCKSIZE_BITS 128
#define KEYSIZE_BITS 128
#define BYTE_SIZE 8
#define ARRAY_SIZE 16
#define ROUNDKEYS_SIZE 176

void print_array(uint8_t *arr, int arr_size) {
    for (size_t i = 0; i < arr_size; i++) {
        printf("%c ", arr[i]);
    }
    printf("\n" );
}

uint8_t* readFilePlainText(FILE *fp) {
    uint8_t c;
    int nch = 0;
    int size = 256;
    uint8_t *buf = malloc(size*sizeof(uint8_t));
    if(buf == NULL)
    {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    while(!feof(fp))
    {
        c = fgetc(fp);
        if(nch >= size-1)
        {
            /* time to make it bigger */
            size += 256;
            buf = realloc(buf, size*sizeof(uint8_t));
            if(buf == NULL)
            {
                fprintf(stderr, "Out of memory\n");
                exit(1);
            }
        }
        if(c == 0xA || c == 0xFF) {
            buf[nch++] = 0x0;
        } else {
            buf[nch++] = c;
        }
    }
    while (nch < size) {
        buf[nch] = 0x0;
        nch++;
    }
    return buf;
}

void deleteInputBuffer(uint8_t *buf) {
    free(buf);
}

void writeFilePlainText(state_t state, FILE *fp_write) {
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            if (state[i][j] == 0x0) {
                fputc(0xA, fp_write);
                break;
            }
            fputc(state[i][j], fp_write);
        }
    }
}

void encryptFile(uint8_t *buf, uint8_t *roundKeys, FILE *fp_write) {
    state_t state;
    int start = 0;
    while(buf[start] != 0x00 || buf[start] != '\000') {
        clearStateMatrix(state);
        setStateMatrix(buf, state, start);
        encrypt(state, roundKeys);
        writeFilePlainText(state, fp_write);
        start = start+16;
    }
}

void decryptFile(uint8_t *buf, uint8_t *roundKeys, FILE *fp_write) {
    state_t state;
    int start = 0;
    while(buf[start] != 0x00 || buf[start] != '\000') {
        setStateMatrix(buf, state, start);
        decrypt(state, roundKeys);
        writeFilePlainText(state, fp_write);
        start = start+16;
        clearStateMatrix(state);
    }
}

char* strConcat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void readAndEncryptFile(char const *file_in, uint8_t *randomKey) {
    FILE *fp_read = fopen(file_in, "r");
    uint8_t *arr = readFilePlainText(fp_read);
    fclose(fp_read);

    //uint8_t randomKeyArray[16];
    //randomStringOfNBits(BLOCKSIZE_BITS, randomKeyArray);
    uint8_t roundKeys[ROUNDKEYS_SIZE];
    generateRoundKeys(randomKey, roundKeys);


    FILE *fp_write;
    fp_write = fopen(strConcat(file_in, "__encrypted.txt"), "w");
    encryptFile(arr, roundKeys, fp_write);
    deleteInputBuffer(arr);
    fclose(fp_write);
}



void readAndDecryptFile(char const *file_in, uint8_t *randomKey) {
    FILE *fp_read = fopen(file_in, "r");
    uint8_t *arr = readFilePlainText(fp_read);
    fclose(fp_read);

    //uint8_t randomKeyArray[16];
    //randomStringOfNBits(BLOCKSIZE_BITS, randomKeyArray);
    uint8_t roundKeys[ROUNDKEYS_SIZE];
    generateRoundKeys(randomKey, roundKeys);

    FILE *fp_write_2;
    fp_write_2 = fopen(strConcat(file_in, "__decrypted.txt"), "w");
    decryptFile(arr, roundKeys, fp_write_2);

    deleteInputBuffer(arr);
    fclose(fp_write_2);
}

void stringKeyToUINTArr(char const*inp_arr, uint8_t *randomKey) {
    for (size_t i = 0; i < 16; i++) {
        randomKey[i] = inp_arr[0];
    }
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("\nminimal-AES\nAuthor: Sebastian\n\n");
        printf("usage:\n\t./AES -f <file_in.txt> -r <randomKey> -d(ecrypt)/-e(ncrypt)\n\n");

    }else if (argc == 6){
        if (argv[2][(strlen(argv[2])-4)] == '.'){
            if(strlen(argv[4]) == 16) {
                if (strcmp(argv[5],"-d") == 0) {
                    printf("Decrypting...\nFile: %s\n", argv[2]);
                    uint8_t randKey[16];
                    stringKeyToUINTArr(argv[4], randKey);
                    printf("Random Key:\n");
                    print_array(randKey, 16);
                    readAndDecryptFile(argv[2], randKey);
                    printf("\n\nFINISHED DECRYPTING\nSee file: %s\n\n", strConcat(argv[2], "__decrypted.txt"));
                    return 0;
                } else if(strcmp(argv[5],"-e") == 0) {
                    printf("Encrypting...\nFile: %s\n", argv[2]);
                    uint8_t randKey[16];
                    stringKeyToUINTArr(argv[4], randKey);
                    printf("Random Key:\n");
                    print_array(randKey, 16);
                    readAndEncryptFile(argv[2], randKey);
                    printf("\n\nFINISHED ENCRYPTING\nSee file: %s\n\n", strConcat(argv[2], "__encrypted.txt"));
                    return 0;
                }
            }
        }
        printf("\nERROR: Make sure you provide the right arguments.\n\n\t./AES -f <file_in> -r <randomKey> -d(ecrypt)/-e(ncrypt)\n\n");

    }
    return 0;
}
