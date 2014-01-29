#!/bin/bash

brasdor=0
fundy=0
glooscap=0
placentia=1

files="cleanup.sh run.sh dpd.inp run.inp sub.sh README analysis"
srcfiles="src/all.c src/init src/main.c src/makefile src/sim src/struct"

if [ $brasdor == 1 ]; then
    echo "Copying to brasdor"
    scp -r $files tdunn@brasdor.ace-net.ca:/home/tdunn/dpd
    scp -r $srcfiles tdunn@brasdor.ace-net.ca:/home/tdunn/dpd/src
fi

if [ $fundy == 1 ]; then
    echo "Copying to fundy"
    scp -r $files tdunn@fundy.ace-net.ca:/home/tdunn/dpd
    scp -r $srcfiles tdunn@fundy.ace-net.ca:/home/tdunn/dpd/src
fi

if [ $glooscap == 1 ]; then
    echo "Copying to glooscap"
    scp -r $files tdunn@glooscap.ace-net.ca:/home/tdunn/dpd
    scp -r $srcfiles tdunn@glooscap.ace-net.ca:/home/tdunn/dpd/src
fi

if [ $placentia == 1 ]; then
    echo "Copying to placentia"
    scp -r $files tdunn@placentia.ace-net.ca:/home/tdunn/dpd
    scp -r $srcfiles tdunn@placentia.ace-net.ca:/home/tdunn/dpd/src
fi

