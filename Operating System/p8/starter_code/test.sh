#!/bin/bash
echo ========= test 1 ========= > output.txt
./app1 >> output.txt
echo ========= test 2 ========= >> output.txt
./app2a >> output.txt ; ./app2b >> output.txt
echo ========= test 3 ========= >> output.txt
./app3 >> output.txt
echo ========= test 4 ========= >> output.txt
./app4 >> output.txt
diff output.txt expected.txt