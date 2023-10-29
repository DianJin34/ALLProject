all: badger_fortune.c
	gcc badger_fortune.c -Wall -std=gnu99 -m32 -o badger_fortune
	./badger_fortune -f fortunes.txt -b batch.txt
	~cs537-1/tests/P1/test-badger-fortune.csh
