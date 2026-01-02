#!/bin/bash

# Configuration for Bimodal Simulation Sweep
SIMULATOR="./sim"
TRACE_FILE="gcc_trace.txt"
OUTPUT_FILE="bimodal_results.txt"

# Check if the simulator and trace file exist
if [ ! -f "$SIMULATOR" ]; then
    echo "Error: Simulator '$SIMULATOR' not found. Please ensure it is in the current directory." >&2
    exit 1
fi

if [ ! -f "$TRACE_FILE" ]; then
    echo "Error: Trace file '$TRACE_FILE' not found." >&2
    exit 1
fi

# Function to run the simulation and extract the required data
run_bimodal_sim() {
    local m=$1
    
    # Run the simulator and capture all output
    local OUTPUT
    OUTPUT=$("$SIMULATOR" bimodal "$m" "$TRACE_FILE" 2>&1)
    
    # Extract misprediction rate, removing the trailing '%' symbol
    # The output format for Bimodal is simpler than Gshare's two-line output
    local RATE
    RATE=$(echo "$OUTPUT" | grep "misprediction rate" | awk '{print $NF}' | sed 's/%//')
    
    # Check if extraction was successful
    if [ -z "$RATE" ]; then
        echo "Warning: Simulation failed or output format was unexpected for m=$m. Skipping." >&2
        return 1
    fi
    
    # Output the data in CSV format: m, misprediction_rate_percentage
    echo "$m,$RATE"
}

# Start of main script execution
echo "Starting Bimodal simulation sweep for m=7 to m=20 using $TRACE_FILE."

# Clear existing file and write header line
echo "m,misprediction_rate" > "$OUTPUT_FILE"

# Iterate over predictor size m (7 to 20)
for m in {7..20}; do
    echo "Simulating bimodal $m..."
    
    # Run the simulation and append the results to the output file
    run_bimodal_sim "$m" >> "$OUTPUT_FILE"
done

echo ""
echo "Bimodal simulation sweep complete."
echo "Results saved to '$OUTPUT_FILE'."
echo "Total entries: $(cat "$OUTPUT_FILE" | wc -l) (14 data points + 1 header)."
