aes: main.c cryptomath.c
	gcc -o AES main.c cryptomath.c -fno-stack-protector
test: cryptomath.c testcryptomath.c
	gcc -Wall -c cryptomath.c
	gcc -Wall -L/usr/local/lib -o crypto_test testcryptomath.c cryptomath.o -lcunit
	./crypto_test
clean:
	rm -f *.o AES crypto_test
dbg: main.c cryptomath.c
	gcc -g -o AES main.c cryptomath.c -fno-stack-protector
valgrind:
	valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./AES
