run:
	gcc myMagicSquare.c -Wall -m32 -std=gnu99 -o myMagicSquare
	./myMagicSquare

compile:
	gcc myMagicSquare.c -Wall -m32 -std=gnu99 -o myMagicSquare

leak:
	gcc myMagicSquare.c -g -Wall -m32 -std=gnu99 -o myMagicSquare
	valgrind --leak-check=full --error-exitcode=1 ./myMagicSquare 3.txt