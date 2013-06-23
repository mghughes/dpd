#!/bin/bash

#
# script for cleaning up/organizing ouput of dpd simulation program
#
# Copyright (C) Matthieu Hughes, 2013
# matthieuhughes.1@gmail.com
#
# last update: 21/06/13, Matthieu Hughes
#		removed structure factor and rdf stuff
#

# NOTES:
#	runs in parent working dir (with job# dirs)

# exit prematurely if any command returns error code
set -e
set -o pipefail

echo "Scanning run.inp"
version=$(cat run.inp | grep 'version' | awk '{print $1}')
njobs=$(cat run.inp | grep 'njobs' | awk '{print $1}')
filesperjob=$(cat run.inp | grep 'filesperjob' | awk '{print $1}')
foldername=$(cat run.inp | grep 'foldername' | awk '{print $1}')

# find all data files
echo "Searching for files"
for ((u=1;u<=$njobs;u++)); do
	
    jobNum=$(printf %02d $u)
    if [ -d job"$jobNum" ]; then # enter jobNum dir if it exists
        cd job"$jobNum"
	
	# different file name styles filesperjob==1 (don't append -01 to end)
	if [ $filesperjob = 1 ]; then
	    if [ -d "01" ]; then # enter the dir if it exists
		cd "01"

		# find all files not named *.mon and move to parentDir
		find -type f -not -name "*.mon" -exec mv '{}' ./../../'{}'-"$jobNum" \;
		cd ..
	    fi # end of if 01 dir exists

	else # do append -01, -02 etc. if filesperjob>1
	    for ((w=1;w<=$filesperjob;w++)); do
				
	        simNum=$(printf %02d $w)
		if [ -d "$simNum" ]; then # enter dir if it exists
		    cd "$simNum"
		    find -type f -not -name "*.mon" -exec mv '{}' ./../../'{}'-"$(printf %02d $u)"-"$(printf %02d $w)" \;
		    cd ..
		fi # end of if simNum dir exists
	    done # end of loop through filesperjob	
	fi # end of if-else filesperjob==1

	cd ..
    fi # end of if jobNum dir exists 
done # end of loop through jobs

# create new folders for files
echo "Creating main folders"
for dir in log rg re nseg; do
    mkdir "$dir"
    mv "$dir"-*  "$dir"/
done

# read dpd.inp for optional folders (struct, dens, etc.)
echo "Scanning dpd.inp"
dens=$(cat dpd.inp | grep 'ydens' | awk '{print $1}')
densxy=$(cat dpd.inp | grep 'ydensxy' | awk '{print $1}')
aveTemp=$(cat dpd.inp | grep 'yaveTemp' | awk '{print $1}')
monPos=$(cat dpd.inp | grep 'ymonPos' | awk '{print $1}')
poreWidth=$(cat dpd.inp | grep 'poreWidth' | awk '{print $1}')

if [ "$poreWidth" = "0.0" ]; then
    re_cis=0
    rg_cis=0
    re_trans=0
    rg_trans=0
else
    re_cis=1
    rg_cis=1
    re_trans=1
    rg_trans=1	
fi

# store values and names in arrays
folderVal=( $dens $densxy $rdf $struct $aveTemp $monPos $re_cis $rg_cis $re_trans $rg_trans )
folderName=( "dens" "densxy" "rdf" "struct" "aveTemp" "monPos" "re_cis" "rg_cis" "re_trans" "rg_trans" )

# loop through arrays and create folders
echo "Creating optional folders"
for (( u=0; u<${#folderVal[@]}; u++ )); do
    if [ ${folderVal[$u]} = 1 ]; then
        mkdir "${folderName[$u]}"
	mv "${folderName[$u]}"-* "${folderName[$u]}"/
    fi
done

# create file of runtimes and transTimes
echo "Finding runtimes and transTimes"
cd log
for l in log-*; do 
    cat "$l" | grep 'runtime' | awk '{print $1}' >> ../runtimes
    cat "$l" | grep 'transTime' | awk '{print $1}' >> ../transTimes
done
cd ..
