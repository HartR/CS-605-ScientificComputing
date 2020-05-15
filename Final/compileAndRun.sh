#!/bin/bash

chmod 600 /home/605/russell/.ssh/id_rsa
git pull
mpic++ -c matrix_multiply.c -o main1.o
nvcc -arch=sm_30 -c cuda_multiply.cu -o cm1.o
mpic++ main1.o cm1.o -lcudart -L/usr/local/cuda/lib64 -o final.binary
if [ -f output.txt ]; then
    mv output.txt oldoutput.txt
fi
qsub batch.final
rm *.o
