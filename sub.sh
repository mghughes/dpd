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
# last update: 20/10/13, Taylor Dunn
#		added gammaPore/sigmaPore and Amf_cis/trans parameters
#

#exit prematurely if any command returns error code
set -e			
set -o pipefail

dir=/home/tdunn/
version=9.5

#TODO params
Aff=25.0
Amm=25.0
Amf_cis=15.0
Amf_trans=15.0
sigma=3.0
gamma=4.5
# sqrt(0.2) = 0.4472
# sqrt(2) = 1.414
# sqrt(20) = 4.472
# sqrt(50) = 7.071
# sqrt(200) = 14.142
sigmaPore=3.0
gammaPore=4.5
K=40.0
fDrive=0.0

Nmon=19; Lx=6.4; Ly=6.4; Lz=15.4
#Nmon=29; Lx=7.3; Ly=7.3; Lz=17.8
#Nmon=39; Lx=8.2; Ly=8.2; Lz=20.2
#Nmon=49; Lx=9.1; Ly=9.1; Lz=22.6
density=3.0

wallDensity=3.0
wallLayers=3
poreWidth=1.3867

dt=0.04
tMax=40000.0
eqTime=200.0
freqSamp=1
freqOut=1000

ydens=0
ydensxy=0
yaveTemp=0
ymonPos=0


#TODO jobs info
njobs=40
filesperjob=10

#TODO parent dir for job data
foldername=exp6/c"$Amf_cis"t"$Amf_trans"/n"$Nmon"

# compile program
cd "$dir"/dpd/src
make
mv dpd.run ../
cd ..

# make dpd.inp in dir/dpd
echo -e "$Aff\tAff\n$Amm\tAmm\n$Amf_cis\tAmf_cis\n$Amf_trans\tAmf_trans" > dpd.inp
echo -e "$sigma\tsigma\n$gamma\tgamma" >> dpd.inp
echo -e "$sigmaPore\tsigmaPore\n$gammaPore\tgammaPore" >> dpd.inp
echo -e "$K\tK\n$fDrive\tfDrive" >> dpd.inp

echo -e "$Nmon\tNmon\n$density\tdensity" >> dpd.inp
echo -e "$Lx\tLx\n$Ly\tLy\n$Lz\tLz" >> dpd.inp

echo -e "$wallDensity\twallDensity" >> dpd.inp
echo -e "$wallLayers\twallLayers\n$poreWidth\tporeWidth" >> dpd.inp

echo -e "$dt\tdt\n$tMax\ttMax\n$eqTime\teqTime" >> dpd.inp
echo -e "$freqSamp\tfreqSamp\n$freqOut\tfreqOut" >> dpd.inp

echo -e "$ydens\tydens\n$yaveTemp\tyaveTemp\n$ymonPos\tymonPos" >> dpd.inp

# make run.inp in dir/src
echo -e "$version\tversion\n$njobs\tnjobs\n$filesperjob\tfilesperjob\n$foldername\tfoldername" > run.inp

# make parent output dir if not in existence
cd "$dir"
wdirPar="$dir"/scratch/dpd/"$foldername"
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
    qsub -cwd -l h_vmem=1G -l h_rt=48:00:00 run.sh #TODO

done
