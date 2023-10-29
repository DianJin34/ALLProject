make
./psort ~cs537-1/tests/P6/test-large/test-large.in out.img 16
~cs537-1/tests/P6/rcheck out.img ~cs537-1/tests/P6/test-large/test-large.out
./psort ~cs537-1/tests/P6/test-large2/test-large2.in out.img 16
~cs537-1/tests/P6/rcheck out.img ~cs537-1/tests/P6/test-large2/test-large2.out
./psort ~cs537-1/tests/P6/test-medium/test-medium-in.txt out.img 16
~cs537-1/tests/P6/rcheck out.img ~cs537-1/tests/P6/test-medium/test-medium-out.txt
./psort ~cs537-1/tests/P6/test-medium2/test-medium2-in.txt out.img 16
~cs537-1/tests/P6/rcheck out.img ~cs537-1/tests/P6/test-medium2/test-medium2-out.txt
./psort ~cs537-1/tests/P6/test-small/test-small-in.txt out.img 16
~cs537-1/tests/P6/rcheck out.img ~cs537-1/tests/P6/test-small/test-small-out.txt
./psort ~cs537-1/tests/P6/test-small2/test-small2-in.txt out.img 16
~cs537-1/tests/P6/rcheck out.img ~cs537-1/tests/P6/test-small2/test-small2-out.txt