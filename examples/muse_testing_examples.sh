#!/bin/bash

for test in `cat test.txt`

do

cd ${test}/script

./10_test_MUSE_${test}.sh project
pwd
cd $OLDPWD
pwd
done
