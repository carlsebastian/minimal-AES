#include <stdint.h>
#include <stdlib.h>
#ifndef CRYPTOMATH_H
#define CRYPTOMATH_H

// state-matrix - 2d-array holding information during encryption
typedef uint8_t state_t[4][4];

int setStateMatrix(uint8_t inp_str[], state_t state, int arr_size);

void addRoundKey(state_t state, uint8_t * roundKeys, size_t roundnr);

void subBytes(state_t state);

void shiftRows(state_t state);

void mixColumns(state_t state);

void encrypt(state_t state, uint8_t *roundkeys);

void generateRoundKeys(uint8_t *input_key, uint8_t *roundKeys);

void randomStringOfNBits(int size, uint8_t randomKeyArray[]);

void print_state_matrix(state_t state);

#endif
