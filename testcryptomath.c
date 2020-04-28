#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
//#include "CUnit/Automated.h"
//#include "CUnit/Console.h"

#include "cryptomath.h"

#include <stdio.h>  // for printf

/* Test Suite setup and cleanup functions: */

//state_t test_state;

int init_suite(void) {
    //test_state = {{},{},{},{}}
    return 0;
}
int clean_suite(void) { return 0; }

/************* Test case functions ****************/

// void test_case_sample(void)
// {
//    CU_ASSERT(CU_TRUE);
//    CU_ASSERT_NOT_EQUAL(2, -1);
//    CU_ASSERT_STRING_EQUAL("string #1", "string #1");
//    CU_ASSERT_STRING_NOT_EQUAL("string #1", "string #2");
//
//    CU_ASSERT(CU_FALSE);
//    CU_ASSERT_EQUAL(2, 3);
//    CU_ASSERT_STRING_NOT_EQUAL("string #1", "string #1");
//    CU_ASSERT_STRING_EQUAL("string #1", "string #2");
// }

void randomkey_test_1(void) {
    //printf("\n - Testing randomkeygen\n");
    uint8_t randomkeyarr[16];
    randomStringOfNBits(128, randomkeyarr);
    CU_ASSERT_EQUAL(sizeof(randomkeyarr), 16);
}

void addRoundKey_test_1(void) {
    state_t state = {
        {'h','e','e','j'},
        {'h','e','e','j'},
        {'h','e','e','j'},
        {'h','e','e','j'}
    };
    uint8_t roundKeys[16] = {
        'h','h','h','h','h','h','h','h','h','h','h','h','h','h','h','h'
    };
    size_t roundnr = 0;
    addRoundKey(state, roundKeys, roundnr);
    for (size_t i = 0; i < 4; i++) {
        //printf("%x\n", state[i][3]);
        CU_ASSERT_EQUAL(state[i][0], 'h' ^ 'h');
        CU_ASSERT_EQUAL(state[i][1], 'e' ^ 'h');
        CU_ASSERT_EQUAL(state[i][2], 'e' ^ 'h');
        CU_ASSERT_EQUAL(state[i][3], 'j' ^ 'h');
    }
}

void setStateMatrix_test_1(void) {
    uint8_t inp[16] = {'h','e','e','j','h','e','e','j','h','e','e','j','h','e','e','j'};
    state_t state;
    setStateMatrix(inp, state, 16);
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            CU_ASSERT_EQUAL(state[i][j], inp[(4*i+j)]);
        }
    }
}

void generateRoundKeys_test_1(void) {
    uint8_t randkey[16] = {'T','h','a','t','s',' ','m','y',' ','K','u','n','g',' ','F','u'};
    uint8_t roundKeys[176];
    generateRoundKeys(randkey, roundKeys);
    uint8_t ans_arr[176] = {
        0x54,0x68,0x61,0x74,0x73,0x20,0x6D,0x79,0x20,0x4B,0x75,0x6E,0x67,0x20,0x46,0x75,
        0xE2,0x32,0xFC,0xF1,0x91,0x12,0x91,0x88,0xB1,0x59,0xE4,0xE6,0xD6,0x79,0xA2,0x93,
        0x56,0x08,0x20,0x07,0xC7,0x1A,0xB1,0x8F,0x76,0x43,0x55,0x69,0xA0,0x3A,0xF7,0xFA,
        0xD2,0x60,0x0D,0xE7,0x15,0x7A,0xBC,0x68,0x63,0x39,0xE9,0x01,0xC3,0x03,0x1E,0xFB,
        0xA1,0x12,0x02,0xC9,0xB4,0x68,0xBE,0xA1,0xD7,0x51,0x57,0xA0,0x14,0x52,0x49,0x5B,
        0xB1,0x29,0x3B,0x33,0x05,0x41,0x85,0x92,0xD2,0x10,0xD2,0x32,0xC6,0x42,0x9B,0x69,
        0xBD,0x3D,0xC2,0x87,0xB8,0x7C,0x47,0x15,0x6A,0x6C,0x95,0x27,0xAC,0x2E,0x0E,0x4E,
        0xCC,0x96,0xED,0x16,0x74,0xEA,0xAA,0x03,0x1E,0x86,0x3F,0x24,0xB2,0xA8,0x31,0x6A,
        0x8E,0x51,0xEF,0x21,0xFA,0xBB,0x45,0x22,0xE4,0x3D,0x7A,0x06,0x56,0x95,0x4B,0x6C,
        0xBF,0xE2,0xBF,0x90,0x45,0x59,0xFA,0xB2,0xA1,0x64,0x80,0xB4,0xF7,0xF1,0xCB,0xD8,
        0x28,0xFD,0xDE,0xF8,0x6D,0xA4,0x24,0x4A,0xCC,0xC0,0xA4,0xFE,0x3B,0x31,0x6F,0x26
    };
    for (size_t i = 0; i < 176; i++) {
        CU_ASSERT_EQUAL(roundKeys[i], ans_arr[i]);
    }
}

void mixColumns_test_1(void) {
    state_t state = {
        {0x63, 0xEB, 0x9F, 0xA0},
        {0x2F, 0x93, 0x92, 0xC0},
        {0xAF, 0xC7, 0xAB, 0x30},
        {0xA2, 0x20, 0xCB, 0x2B}
    };
    state_t ans = {
        {0xBA, 0x84, 0xE8, 0x1B},
        {0x75, 0xA4, 0x8D, 0x40},
        {0xF4, 0x8D, 0x06, 0x7D},
        {0x7A, 0x32, 0x0E, 0x5D}
    };
    mixColumns(state);
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            //printf("%ld, %ld - %X == %X\n",i,j, state[i][j], ans[i][j]);
            CU_ASSERT_EQUAL(state[i][j], ans[i][j]);
        }
    }

}

void shiftRows_test_1(void) {
    state_t state = {
        {'h','e','e','j'},
        {'h','e','e','j'},
        {'h','e','e','j'},
        {'h','e','e','j'}
    };
    shiftRows(state);
    CU_ASSERT_EQUAL(state[0][0], 0x68);
    CU_ASSERT_EQUAL(state[0][1], 0x65);
    CU_ASSERT_EQUAL(state[0][2], 0x65);
    CU_ASSERT_EQUAL(state[0][3], 0x6a);

    CU_ASSERT_EQUAL(state[1][0], 0x65);
    CU_ASSERT_EQUAL(state[1][1], 0x65);
    CU_ASSERT_EQUAL(state[1][2], 0x6a);
    CU_ASSERT_EQUAL(state[1][3], 0x68);

    CU_ASSERT_EQUAL(state[2][0], 0x65);
    CU_ASSERT_EQUAL(state[2][1], 0x6a);
    CU_ASSERT_EQUAL(state[2][2], 0x68);
    CU_ASSERT_EQUAL(state[2][3], 0x65);

    CU_ASSERT_EQUAL(state[3][0], 0x6a);
    CU_ASSERT_EQUAL(state[3][1], 0x68);
    CU_ASSERT_EQUAL(state[3][2], 0x65);
    CU_ASSERT_EQUAL(state[3][3], 0x65);

}

void subBytes_test_1(void) {
    state_t state = {
        {'h','e','e','j'},
        {'h','e','e','j'},
        {'h','e','e','j'},
        {'h','e','e','j'}
    };
    subBytes(state);
    for (size_t i = 0; i < 4; i++) {
        // 68 65 6a
        CU_ASSERT_EQUAL(state[i][0], 0x45);
        CU_ASSERT_EQUAL(state[i][1], 0x4d);
        CU_ASSERT_EQUAL(state[i][2], 0x4d);
        CU_ASSERT_EQUAL(state[i][3], 0x02);
    }
}

void encrypt_test_1(void) {
    uint8_t k[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    state_t s = {
        {0x32, 0x43, 0xf6, 0xa8},
        {0x88, 0x5a, 0x30, 0x8d},
        {0x31, 0x31, 0x98, 0xa2},
        {0xe0, 0x37, 0x07, 0x34}
    };
    uint8_t keys[176];
    uint8_t ans[16] = {         // What I get atm
        0x39, 0x25, 0x84, 0x1d, //B8 22 FE 47
        0x02, 0xdc, 0x09, 0xfb, //6F 13 F2 CA
        0xdc, 0x11, 0x85, 0x97, //82 11 ED 45 <-- 11 equal
        0x19, 0x6a, 0x0b, 0x32  //E3 37 58 82
    };
    generateRoundKeys(k, keys);
    uint8_t cipher[16];
    encrypt(s, keys, cipher);
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            printf("%ld\n", (i*4)+j);
            CU_ASSERT_EQUAL(s[i][j], ans[(i*4)+j])
        }
    }
}

/************* Test Runner Code goes here **************/

int main ( void )
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if ( CUE_SUCCESS != CU_initialize_registry() )
    return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite( "cryptomath_test_suite", init_suite, clean_suite );
    if ( NULL == pSuite ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ( (NULL == CU_add_test(pSuite, "randomkey_test_1", randomkey_test_1)) ||
    (NULL == CU_add_test(pSuite, "addRoundKey_test_1", addRoundKey_test_1)) ||
    (NULL == CU_add_test(pSuite, "setStateMatrix_test_1", setStateMatrix_test_1)) ||
    (NULL == CU_add_test(pSuite, "generateRoundKeys_test_1", generateRoundKeys_test_1)) ||
    (NULL == CU_add_test(pSuite, "subBytes_test_1", subBytes_test_1)) ||
    (NULL == CU_add_test(pSuite, "shiftRows_test_1", shiftRows_test_1)) ||
    (NULL == CU_add_test(pSuite, "mixColumns_test_1", mixColumns_test_1)) ||
    (NULL == CU_add_test(pSuite, "encrypt_test_1", encrypt_test_1))

)
{
    CU_cleanup_registry();
    return CU_get_error();
}

// Run all tests using the basic interface
CU_basic_set_mode(CU_BRM_VERBOSE);
CU_basic_run_tests();
printf("\n");
CU_basic_show_failures(CU_get_failure_list());
printf("\n\n");
/*
// Run all tests using the automated interface
CU_automated_run_tests();
CU_list_tests_to_file();

// Run all tests using the console interface
CU_console_run_tests();
*/
/* Clean up registry and return */
CU_cleanup_registry();
return CU_get_error();
}
