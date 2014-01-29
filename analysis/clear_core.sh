#!/bin/bash

for dir in job*/*/;
do
  if [ -f "$dir"/core* ]; then
    echo "Core file exists in $dir"
    rm -r "$dir"
  fi
done
