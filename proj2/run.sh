#!/bin/bash

#prl
export PATH=$PATH:/usr/lib64/openmpi/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64/openmpi/lib

#preklad cpp zdrojaku
mpic++ -o parkmaens parkmaens.cpp

#spusteni
mpirun -np 10 -oversubscribe parkmaens
