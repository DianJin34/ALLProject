#!/bin/bash -e

# no test0, because it has some problem

for i in {0,1,3,4,5,6,7}
do 
    echo ============ test $i ============
    name=0$i.img
    ./runscan ./test_disk_images/test_$i/$name output_0$i
    python3 rcheck.py ./output_0$i ./test_disk_images/test_$i/output
done
