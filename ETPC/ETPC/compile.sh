#!/bin/bash
rm program.89z
rm program.9xz
./etpc TestFiles/test.etp
/Users/onurcelebi/development/otherstuff/gcc4ti/trunk/external/a68k/source/A68k -i/Users/onurcelebi/development/otherstuff/gcc4ti/trunk/tigcc/include/ASM/ -d outetp.asm -ooutetp.o

/Users/onurcelebi/development/otherstuff/gcc4ti/trunk/tigcc/ld-tigcc/ld-tigcc outetp.o -o program



#/Users/onurcelebi/development/otherstuff/gcc4ti/trunk/external/a68k/source/A68k -i/Users/onurcelebi/development/otherstuff/gcc4ti/trunk/tigcc/include/ASM/ etplib.asm -oetplib.o
#/Users/onurcelebi/development/otherstuff/gcc4ti/trunk/external/a68k/source/A68k -i/Users/onurcelebi/development/otherstuff/gcc4ti/trunk/tigcc/include/ASM/ outetp.asm -ooutetp.o
#/Users/onurcelebi/development/otherstuff/gcc4ti/trunk/tigcc/ld-tigcc/ld-tigcc etplib.o outetp.o -o program



