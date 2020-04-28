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
        printf("%X ", arr[i]);
    }
    printf("\n" );
}




int main(int argc, char const *argv[]) {
    randomStringOfNBits(BLOCKSIZE_BITS, randomKeyArray);
    uint8_t inp_str[ARRAY_SIZE] = {'h','e','j',' ','h','e','j',' ','h','e','j',' ','h','e','j',' '};
    printf("random Key: \n");
    print_array(randomKeyArray, 16);

    printf("plaintext: \n");
    print_array(inp_str, 16);

    state_t state;
    setStateMatrix(inp_str, state, ARRAY_SIZE);

    uint8_t roundKeys[ROUNDKEYS_SIZE];
    generateRoundKeys(randomKeyArray, roundKeys);
    //print_array(roundKeys, ROUNDKEYS_SIZE);

    uint8_t cipher[ARRAY_SIZE];
    encrypt(state, roundKeys);
    printf("Cipher:\n");
    print_state_matrix(state);


    return 0;
}
