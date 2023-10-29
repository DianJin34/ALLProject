CC = gcc
CFLAGS = -Wall -Wextra -Werror
DFLAGS = -g
DEPENDENCIES.C = read_ext2.c
EXEC = runscan
MAIN.C = runscan.c

default: main

clean:
	rm -f $(EXEC)
	rm -r ./output_00
	rm -r ./output_01
	rm -r ./output_03
	rm -r ./output_04
	rm -r ./output_05
	rm -r ./output_06
	rm -r ./output_07

main: $(MAIN.C)
	$(CC) $(CFLAGS) $(DFLAGS) $(MAIN.C) $(DEPENDENCIES.C) -o $(EXEC)

test: 
	./runscan test_disk_images/test_0/00.img output_00
	python3 rcheck.py ./output_00 ./test_disk_images/test_0/output
	
test1: 
	./runscan test_disk_images/test_1/01.img output_01
	python3 rcheck.py ./output_01 ./test_disk_images/test_1/output
	
test2: 
	./runscan test_disk_images/test_2/02.img output_02
	python3 rcheck.py ./output_07 ./test_disk_images/test_7/output
	
test3: 
	./runscan test_disk_images/test_3/03.img output_03
	python3 rcheck.py ./output_07 ./test_disk_images/test_7/output

test4: 
	./runscan test_disk_images/test_4/04.img output_04
	python3 rcheck.py ./output_07 ./test_disk_images/test_7/output

test5: 
	./runscan test_disk_images/test_5/05.img output_05
	python3 rcheck.py ./output_07 ./test_disk_images/test_7/output

test6: 
	./runscan test_disk_images/test_6/06.img output_06
	python3 rcheck.py ./output_06 ./test_disk_images/test_6/output

test7: 
	./runscan test_disk_images/test_7/07.img output_07
	python3 rcheck.py ./output_07 ./test_disk_images/test_7/output

testall: alltest
alltest: main
	./test.sh > test.out