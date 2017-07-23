#!/bin/bash
rm ../ETPC/Obj/*.gcda   # coverage info


for f in ../ETP-Basic\ files/*.etp
do
    rm program.89z
    rm program.9xz
    rm outetp.asm

    echo "Compiling ETP file: $f"
    ../ETPC/etpc "$f"

    export GCC4TI_PATH="/Users/onurcelebi/development/otherstuff/gcc4ti/trunk"

    $GCC4TI_PATH/external/a68k/source/A68k -i$GCC4TI_PATH/tigcc/include/ASM/ -d outetp.asm -ooutetp.o
    $GCC4TI_PATH/tigcc/ld-tigcc/ld-tigcc outetp.o -o program

    if [ ! -f program.89z ]; then
        echo "File not found! ETP didn't compile $f."
        exit
    fi
done


# exit


# Coverage

export SONAR_SCANNER_PATH="/Users/onurcelebi/Downloads/sonar-scanner-3.0.3.778-macosx"

/usr/local/Cellar/lcov/1.13/bin/lcov --directory ../ETPC/Obj/ \
       --base-directory ../ETPC/ \
       --capture -o cov.info

./lcov_cobertura.py cov.info -o output-for-sonar.xml --demangle
cp sonar-project.properties ../ETPC/
cd ../ETPC/
$SONAR_SCANNER_PATH/bin/sonar-scanner



