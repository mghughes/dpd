#!/bin/bash

set -e
set -o pipefail 

ncis=$(cat log/log* | grep transType | awk '{print $1}' | grep cis | wc -l)
ntrans=$(cat log/log* | grep transType | awk '{print $1}' | grep trans | wc -l)
let ntotal=ncis+ntrans

echo "$ncis/$ntotal" | bc -l
echo "$ntrans/$ntotal" | bc -l
echo "ncis $ncis"
echo "ntrans $ntrans"
echo "ntotal $ntotal"
