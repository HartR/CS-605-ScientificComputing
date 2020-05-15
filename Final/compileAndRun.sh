#!/bin/bash

mpic++ -c matrix_multiply.c -o main1.o
nvcc -arch=sm_30 -c cuda_multiply.cu -o cm1.o
mpic++ main1.o cm1.o -lcudart -L/usr/local/cuda/lib64 -o final.binary
mv output.txt oldoutput.txt
qsub batch.final
rm *.o
