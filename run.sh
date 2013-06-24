#!/bin/bash

#
# run script for dpd simulation program.
#
# Written for use on ACEnet computer clusters
# but should work on any computer/cluster
#
# Copyright (C) Matthieu Hughes, 2013
# matthieuhughes.1@gmail.com
#
# last update: 17/06/13, Matthieu Hughes
#

# NOTES:
# 	runs from job dirs in results so all paths are relative
#	change paths if you want to run it elsewhere

# exit prematurely if a command returns error code
# commented out because we want next simulation to start
#   even if previous one returns !0 (segfault, etc)
#set -e
#set -o pipefail

# read and store run input values
version=$(cat run.inp | grep 'version' | awk '{print $1}')
njobs=$(cat run.inp | grep 'njobs' | awk '{print $1}')
filesperjob=$(cat run.inp | grep 'filesperjob' | awk '{print $1}')
foldername=$(cat run.inp | grep 'foldername' | awk '{print $1}')

for ((u=1;u<=$filesperjob;u++)); do

    # make directory for output
    wdir="$(printf %02d $u)"
    mkdir -p "$wdir"
    cp dpd.run "$wdir"/
    cp dpd.inp "$wdir"/
	
    cd "$wdir"
    ./dpd.run

    rm dpd.inp
    rm dpd.run
	
    cd ..

done

# remove files
rm dpd.run 
rm run.inp
rm dpd.inp
rm run.sh
