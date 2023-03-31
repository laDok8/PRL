#!/bin/bash

#prl
export PATH=$PATH:/usr/lib64/openmpi/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64/openmpi/lib

#pocet cisel bud zadam nebo 10 :)
if [ $# -lt 1 ];then
numbers=10;
else
numbers=$1;
fi;

#preklad cpp zdrojaku
mpic++ -o parsplit parsplit.cpp


#vyrobeni souboru s random cisly
dd if=/dev/random bs=1 count=$numbers of=numbers

#spusteni
mpirun -np $numbers -oversubscribe parsplit

#uklid
rm -f oems numbers
