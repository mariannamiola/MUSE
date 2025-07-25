#!/bin/bash

export n=36

for ((test=1; test<=${n}; test++))
do
    ./10_test_MUSE_20_DGM_SMargherita.sh project -d ../data/Temperatura_Ambiente_merge${test}.csv -p 20_DGM_SMargherita${test}
done
