#include "cryptomath.h"  /* Include the header (not strictly necessary here) */
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define BYTE_SIZE 8
#define nOfWordsKey 4
#define nOfStateColumns 4
#define nRounds 10

// Rijnadel sbox lookuptable
static const uint8_t sbox_table[256] = {
    //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Rijnadel inverse sbox lookuptable
static const uint8_t sbox_inv_table[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

// The round constant word array, Rcon[i], contains the values given by
// x to the power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
static const uint8_t Rcon[11] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

/*
* Function:  clearStateMatrix
* --------------------
* clears every cell of the state
*
*
*/
void clearStateMatrix(state_t state) {
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            state[i][j] = 0x00;
        }
    }
}

/*
* Function:  setStateMatrix
* --------------------
* sets the statematrix with the 16 byte input array
*
*  inp_str: input character array
*  state: 4x4 matrix
*  arr_size: size of the unput array
*
*/
int setStateMatrix(uint8_t *inp_str, state_t state, int start) {
    // int diff = stop-start;
    // if (diff > 16) {
    //     return -1;
    // }
    // for every character in input_string array put them in the state matrix
    int j = 0;
    int inner_start = 0;
    for (int i = start; i < start+16; i++) {
        uint8_t c = inp_str[i];
        int i_inner;
        if (inner_start > 3) {

            i_inner = inner_start/4;
        } else {
            i_inner = 0;
        }
        if (j == 4) {
            j = 0;
        }
        state[i_inner][j] = c;
        j++;
        inner_start++;
    }
    return 1;
}


/*
* Function:  randomStringOfNBits
* --------------------
* Generates a random character array of n bits
*
*  size: number of bits
*  randomKeyArray: array to hold the randomkey
*
*/
void randomStringOfNBits(int size, uint8_t *randomKeyArray) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    time_t t;
    srand((unsigned) time(&t));

    if (size/BYTE_SIZE > 0) {
        for (int n = 0; n < (size/BYTE_SIZE); n++) {
            int key = rand() % (int) (sizeof charset - 1);
            randomKeyArray[n] = charset[key];
        }
    }
}

/*
* Function:  addRoundKey
* --------------------
*
*  xors key for round n to state matrix
*
*  state: current state of cipher
*  roundkeys: the complete array of roundkeys
*  roundnr: current round
*
*/
void addRoundKey(state_t state, uint8_t * roundKeys, size_t roundnr) {
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            state[i][j] ^= roundKeys[(roundnr * nOfStateColumns * 4) + (i * nOfStateColumns) + j];
        }
    }
}

/*
* Function:  getInvSubstitute
* --------------------
*  returns the inverse substitue value of input_char
*
*  input_char: char for which to get the inverse substitute
*
*  returns: the substite of input_char
*/
uint8_t getInvSubstitute(uint8_t input_char) {
    return sbox_inv_table[input_char];
}

/*
* Function:  getSubstitute
* --------------------
*  returns the substitue value of input_char
*
*  input_char: char for which to get the substitute
*
*  returns: the substite of input_char
*/
uint8_t getSubstitute(uint8_t input_char) {
    return sbox_table[input_char];
}

/*
* Function:  subBytes
* --------------------
*
*  substitutes every byte in the state matrix
*
*  state: the current state of the matrix
*
*/
void subBytes(state_t state) {
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            state[i][j] = getSubstitute(state[i][j]);
        }
    }
}

/*
* Function:  rotWord
* --------------------
*  rotates a word(4 bytes) in the left direction
*
*  input_key: key consisting of 4 bytes
*
*/
void rotWord(uint8_t *input_key) {
    const uint8_t tmp= input_key[0];
    input_key[0] = input_key[1];
    input_key[1] = input_key[2];
    input_key[2] = input_key[3];
    input_key[3] = tmp;
}

/*
* Function:  subWord
* --------------------
*  substitutes a word(4 bytes) using the sbox-table
*
*  input_key: the 4 bytes that will be substituted
*
*/
void subWord(uint8_t *input_key) {
    input_key[0] = getSubstitute(input_key[0]);
    input_key[1] = getSubstitute(input_key[1]);
    input_key[2] = getSubstitute(input_key[2]);
    input_key[3] = getSubstitute(input_key[3]);
}

/*
* Function:  shiftRows
* --------------------
*  shifts the rows of the state-matrix
*
*  state: the current state of the matrix
*
*/
void shiftRows(state_t state) {
    uint8_t temp;

    // Rotate first row 1 columns to left
    temp        = state[0][1];
    state[0][1] = state[1][1];
    state[1][1] = state[2][1];
    state[2][1] = state[3][1];
    state[3][1] = temp;

    // Rotate second row 2 columns to left
    temp        = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = temp;

    temp        = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = temp;

    // Rotate third row 3 columns to left
    temp        = state[0][3];
    state[0][3] = state[3][3];
    state[3][3] = state[2][3];
    state[2][3] = state[1][3];
    state[1][3] = temp;
}

/*
* Function:  mixOneColumn
* --------------------
*  runs Rijndaels mixcolumn-function on 4 bytes(one column). Taken from wikipedia
*
*  inp_arr: one column in the state-matrix
*
*/
void mixOneColumn(uint8_t *inp_arr) {
    unsigned char a[4];
    unsigned char b[4];
    unsigned char c;
    unsigned char h;
    /* The array 'a' is simply a copy of the input array 'r'
    * The array 'b' is each element of the array 'a' multiplied by 2
    * in Rijndael's Galois field
    * a[n] ^ b[n] is element n multiplied by 3 in Rijndael's Galois field */
    for (c = 0; c < 4; c++) {
        a[c] = inp_arr[c];
        /* h is 0xff if the high bit of r[c] is set, 0 otherwise */
        h = (unsigned char)((signed char)inp_arr[c] >> 7); /* arithmetic right shift, thus shifting in either zeros or ones */
        b[c] = inp_arr[c] << 1; /* implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b in the next line */
        b[c] ^= 0x1B & h; /* Rijndael's Galois field */
    }
    inp_arr[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
    inp_arr[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
    inp_arr[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
    inp_arr[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */

}

/*
* Function:  mixColumns
* --------------------
*  runs mixcolumn on each column in the state-matrix
*
*  state: the current state of the matrix
*
*/
void mixColumns(state_t state) {
    for (size_t j = 0; j < 4; j++) {
        mixOneColumn(state[j]);
    }
}

/*
* Function:  multiplyBitWise
* --------------------
*
*  a: first bit term
*  b: second bit term
*
*/
uint8_t multiplyBitWise(uint8_t a, uint8_t b) {
    unsigned char p = 0;
    unsigned char counter;
    unsigned char hi_bit_set;
    for(counter = 0; counter < 8; counter++) {
        if((b & 1) == 1)
        p ^= a;
        hi_bit_set = (a & 0x80);
        a <<= 1;
        if(hi_bit_set == 0x80)
        a ^= 0x1b;
        b >>= 1;
    }
    return p;
}


/*
* Function:  generateRoundKeys
* --------------------
*  Generates all the roundkeys needed for encryption/decryption
*
*  input_key: the original key to expand
*  roundKeys: the array to hold all roundkeys needed
*
*/

void generateRoundKeys(uint8_t *input_key, uint8_t *roundKeys) {
    //
    uint8_t tempa[4];
    unsigned k,j;
    for (size_t i = 0; i < nOfWordsKey; i++) {
        // if i < 4 (first 4 words)
        roundKeys[(i * 4) + 0] = input_key[(i * 4) + 0];
        roundKeys[(i * 4) + 1] = input_key[(i * 4) + 1];
        roundKeys[(i * 4) + 2] = input_key[(i * 4) + 2];
        roundKeys[(i * 4) + 3] = input_key[(i * 4) + 3];
    }
    for(size_t i = nOfWordsKey; i < nOfStateColumns * (nRounds+1); i++) {
        k = (i - 1) * 4;
        tempa[0]=roundKeys[k + 0];
        tempa[1]=roundKeys[k + 1];
        tempa[2]=roundKeys[k + 2];
        tempa[3]=roundKeys[k + 3];

        if ( i % nOfWordsKey == 0 ) {
            rotWord(tempa);
            subWord(tempa);
            tempa[0] = tempa[0] ^ Rcon[i/nOfWordsKey];
        }
        j = i * 4; k=(i - nOfWordsKey) * 4;
        roundKeys[j + 0] = roundKeys[k + 0] ^ tempa[0];
        roundKeys[j + 1] = roundKeys[k + 1] ^ tempa[1];
        roundKeys[j + 2] = roundKeys[k + 2] ^ tempa[2];
        roundKeys[j + 3] = roundKeys[k + 3] ^ tempa[3];
    }
}

/*
* Function:  invSubBytes
* --------------------
*  The inverse of the SubBytes function
*
*  state: the current state of the matrix
*
*/
void invSubBytes(state_t state) {
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            state[i][j] = getInvSubstitute(state[i][j]);
        }
    }
}

/*
* Function:  invShiftRows
* --------------------
*  The inverse of the ShiftRows function
*
*  state: the current state of the matrix
*
*/
void invShiftRows(state_t state) {
    uint8_t temp;

    // Rotate first row 1 columns to right
    temp        = state[3][1];
    state[3][1] = state[2][1];
    state[2][1] = state[1][1];
    state[1][1] = state[0][1];
    state[0][1] = temp;

    // Rotate second row 2 columns to right
    temp        = state[3][2];
    state[3][2] = state[1][2];
    state[1][2] = temp;

    temp        = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = temp;

    // Rotate third row 3 columns to right
    temp        = state[3][3];
    state[3][3] = state[0][3];
    state[0][3] = state[1][3];
    state[1][3] = state[2][3];
    state[2][3] = temp;
}

/*
* Function:  invMixColumns
* --------------------
*  runs inverse mixcolumn on each column in the state-matrix
*
*  state: the current state of the matrix
*
*/
void invMixColumns(state_t state) {
    for (size_t i = 0; i < 4; i++) {
        unsigned char a[4];
        unsigned char c;
        for(c=0;c<4;c++) {
            a[c] = state[i][c];
        }
        state[i][0] = multiplyBitWise(a[0],14) ^ multiplyBitWise(a[3],9) ^ multiplyBitWise(a[2],13) ^ multiplyBitWise(a[1],11);
        state[i][1] = multiplyBitWise(a[1],14) ^ multiplyBitWise(a[0],9) ^ multiplyBitWise(a[3],13) ^ multiplyBitWise(a[2],11);
        state[i][2] = multiplyBitWise(a[2],14) ^ multiplyBitWise(a[1],9) ^ multiplyBitWise(a[0],13) ^ multiplyBitWise(a[3],11);
        state[i][3] = multiplyBitWise(a[3],14) ^ multiplyBitWise(a[2],9) ^ multiplyBitWise(a[1],13) ^ multiplyBitWise(a[0],11);
    }
}

/*
* Function:  print_state_matrix
* --------------------
*  for testing purposes. To view the current state of the matrix
*
*  state: the current state of the matrix
*
*/
void print_state_matrix(state_t state) {
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            printf("%X ", state[i][j]);
        }
    }
    // printf("\n");
    // for (size_t i = 0; i < 4; i++) {
    //     for (size_t j = 0; j < 4; j++) {
    //         printf("%c ", state[i][j]);
    //     }
    // }
    printf("\n");

}

/*
* Function:  encrypt
* --------------------
*  encrypts the provided message held in the state-matrix
*
*  state: the current state of the matrix
*  roundKeys: the array that holds all roundkeys needed
*
*/
void encrypt(state_t state, uint8_t *roundkeys) {
    size_t roundnr = 0;
    // Addroundkey
    addRoundKey(state, roundkeys, roundnr);
    // For loop
    for (roundnr = 1; roundnr < nRounds; roundnr++) {
        // SubBytes
        subBytes(state);
        // ShiftRows
        shiftRows(state);
        // MixColumns
        mixColumns(state);
        // AddRoundKey
        addRoundKey(state, roundkeys, roundnr);
    }
    // Last Round
    // SubBytes
    subBytes(state);

    // ShiftRows
    shiftRows(state);

    // AddRoundKey
    addRoundKey(state, roundkeys, 10);
}

/*
* Function:  decrypt
* --------------------
* Description
*
*  arg1:
*
*  returns:
*/
void decrypt(state_t state, uint8_t *roundkeys) {
    size_t roundnr = 10;
    // Addroundkey
    addRoundKey(state, roundkeys, roundnr);
    // For loop
    for (roundnr = 9; roundnr > 0; roundnr--) {
        // InvShiftRows
        invShiftRows(state);
        // InvSubBytes
        invSubBytes(state);
        // AddRoundKey
        addRoundKey(state, roundkeys, roundnr);
        // InvMixColumns
        invMixColumns(state);
    }
    // Last Round
    // ShiftRows
    invShiftRows(state);

    // SubBytes
    invSubBytes(state);

    // AddRoundKey
    addRoundKey(state, roundkeys, 0);
}
