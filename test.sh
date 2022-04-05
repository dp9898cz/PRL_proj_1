#!/bin/bash

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o oems oems.cpp

#vyrobeni souboru s random cisly
dd if=/dev/random bs=1 count=8 of=numbers

#spusteni
mpirun --prefix /usr/local/share/OpenMPI -oversubscribe -np 19 oems 

#uklid
rm -f oems numbers