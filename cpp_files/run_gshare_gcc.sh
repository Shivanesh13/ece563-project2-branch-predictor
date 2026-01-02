#!/bin/bash

# ===============================
# Run gshare simulation for gcc_trace.txt
# ===============================

TRACE="gcc_trace.txt"
OUTPUT_FILE="gcc_gshare_results.txt"

# Clear previous output
> $OUTPUT_FILE

echo "Running gshare predictor simulations for $TRACE..."
echo "-------------------------------------------------" >> $OUTPUT_FILE
echo "m,n,misprediction_rate" >> $OUTPUT_FILE

# Loop over m = 7 to 20
for ((m=7; m<=20; m++))
do
    # For each m, n runs from 0 to m
    for ((n=0; n<=m; n++))
    do
        echo "Simulating m=$m, n=$n ..."
        # Run simulator and capture misprediction rate
        RESULT=$(./sim gshare $m $n $TRACE | grep -i "misprediction" | grep -oE "[0-9.]+")
        # In case output format differs, adjust grep pattern
        if [ -z "$RESULT" ]; then
            RESULT="NaN"
        fi
        echo "$m,$n,$RESULT" >> $OUTPUT_FILE
    done
done

echo "-------------------------------------------------" >> $OUTPUT_FILE
echo "All results stored in $OUTPUT_FILE"
