aes: main.c cryptomath.c
	gcc -o AES main.c cryptomath.c -fno-stack-protector
test: cryptomath.c testcryptomath.c
	gcc -Wall -c cryptomath.c
	gcc -Wall -L/usr/local/lib -o crypto_test testcryptomath.c cryptomath.o -lcunit
	./crypto_test
