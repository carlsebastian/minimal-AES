# minimal-AES
[Inspired by Tiny-AES](https://github.com/kokke/tiny-AES-c)

My attempt on coding my own version of AES.

* 128bit keys (Maybe support for 256 in the future)
* State_t is a 4x4 matrix where the 16 byte blocks are stored for the manipulations through encryption/decryption.

* cryptomath - Currently the file where all the AES-related functions lie
* main - just a driver-file to run the program

## To run
* Compile:  `make aes`
* Run: `./AES`

## To test
`make test`

## Clean
`make clean`
