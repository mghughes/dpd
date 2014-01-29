cat job*/*/log | grep 'transTime' | awk '{print $1}' >> transTimes
cat log/log* | grep 'transTime' | awk '{print $1}' >> transTimes
