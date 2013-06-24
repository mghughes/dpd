#!/bin/bash

#
# script for submitting dpd jobs to clusters
#
# Written for use on ACEnet computer clusters,
# but should work for other systems using Sun Grid Engine
#
# Copyright (C) Matthieu Hughes, 2013
# matthieuhughes.1@gmail.com
#
# last update: 21/06/13, Matthieu Hughes
#		removed structure factor and rdf stuff
#

#exit prematurely if any command returns error code
set -e			
set -o pipefail

dir=/home/mhughes/
version=9.3

#TODO params
Aff=25.0
Amm=25.0
Amf=15.0
sigma=3.0
gamma=4.5
K=40.0
fDrive=0.0

Nmon=29
density=3.0
Lx=11.4
Ly=11.4
Lz=18.6

wallDensity=3.0
wallLayers=2
poreWidth=2.4

dt=0.04
tMax=2000.0
eqTime=200.0
freqSamp=1
freqOut=1000
seed=0

ydens=0
ydensxy=0
yaveTemp=0
ymonPos=0


#TODO jobs info
njobs=10
filesperjob=200

#TODO parent dir for job data
foldername=testing

# compile program
cd "$dir"/dpd/src
make
mv dpd.run ../
cd ..

# make dpd.inp in dir/dpd
echo -e "$Aff\tAff\n$Amm\tAmm\n$Amf\tAmf" > dpd.inp
echo -e "$sigma\tsigma\n$gamma\tgamma\n$K\tK\n$fDrive\tfDrive" >> dpd.inp

echo -e "$Nmon\tNmon\n$density\tdensity" >> dpd.inp
echo -e "$Lx\tLx\n$Ly\tLy\n$Lz\tLz" >> dpd.inp

echo -e "$wallDensity\twallDensity" >> dpd.inp
echo -e "$wallLayers\twallLayers\n$poreWidth\tporeWidth" >> dpd.inp

echo -e "$dt\tdt\n$tMax\ttMax\n$eqTime\teqTime" >> dpd.inp
echo -e "$freqSamp\tfreqSamp\n$freqOut\tfreqOut\n$seed\tseed" >> dpd.inp

echo -e "$ydens\tydens\n$ydensxy\tydensxy" >> dpd.inp
echo -e "$yaveTemp\tyaveTemp\n$ymonPos\tymonPos" >> dpd.inp

# make run.inp in dir/src
echo -e "$version\tversion\n$njobs\tnjobs\n$filesperjob\tfilesperjob\n$foldername\tfoldername" > run.inp

# make parent output dir if not in existence
cd "$dir"
wdirPar="$dir"/scratch/"$foldername"
mkdir -p "$wdirPar"

# copy cleanup, dpd.inp and run.inp to parent dir
cp "$dir"/dpd/cleanup.sh "$wdirPar"/
cp "$dir"/dpd/run.inp "$wdirPar"/
cp "$dir"/dpd/dpd.inp "$wdirPar"/
cd "$wdirPar"/

for ((u=1;u<=$njobs;u++)); do

    # make working dir child for each job
    wdir="$wdirPar"/job"$(printf %02d $u)"
    mkdir -p "$wdir"

    cp "$dir"/dpd/dpd.run "$wdir"/
    cp "$dir"/dpd/dpd.inp "$wdir"/
    cp "$dir"/dpd/run.sh "$wdir"/
    cp "$dir"/dpd/run.inp "$wdir"/

    cd "$wdir"
    #echo -e "$(printf %02d $u)\tjobnumber" >> run.inp	
	
    # submit jobs
    qsub -cwd -l h_rt=48:00:00 run.sh #TODO

done
