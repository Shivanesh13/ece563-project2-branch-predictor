#!/usr/bin/env bash
# =============================================================
# Script: analyze_bimodal_minima.sh
# Purpose: Find the minimum misprediction rate and corresponding
#          m value for each benchmark from the bimodal results.
# =============================================================

set -euo pipefail

RESULT_DIR="results"
BENCHMARKS=("gcc_trace" "jpeg_trace" "perl_trace")

echo "=============================================================="
echo "   Bimodal Predictor: Minimum Misprediction Rate Analysis"
echo "=============================================================="
printf "%-15s %-10s %-20s\n" "Benchmark" "m_min" "Min Misprediction Rate (%)"
echo "--------------------------------------------------------------"

for BENCH in "${BENCHMARKS[@]}"; do
    MIN_RATE=999999
    MIN_M=-1

    for ((M=7; M<=20; M++)); do
        FILE="${RESULT_DIR}/${BENCH}_m${M}.log"
        if [[ ! -f "$FILE" ]]; then
            echo "Warning: Missing $FILE" >&2
            continue
        fi

        # Extract misprediction rate number (supports formats like "2.34%" or "0.0234")
        RATE_LINE=$(grep -i "misprediction rate" "$FILE" | tail -n 1 || true)
        if [[ -z "$RATE_LINE" ]]; then
            # Try to compute if numbers of predictions/mispredictions exist
            MIS=$(grep -i "number of mispredictions" "$FILE" | grep -Eo '[0-9]+' | tail -n 1 || echo "")
            TOT=$(grep -i "number of predictions" "$FILE" | grep -Eo '[0-9]+' | tail -n 1 || echo "")
            if [[ -n "$MIS" && -n "$TOT" && "$TOT" -ne 0 ]]; then
                RATE=$(awk -v mis="$MIS" -v tot="$TOT" 'BEGIN { printf "%.6f", (mis*100.0/tot) }')
            else
                continue
            fi
        else
            # Extract numeric part
            RATE=$(echo "$RATE_LINE" | grep -Eo '[0-9]+([.][0-9]+)?' | head -n 1)
            # If rate looks like a fraction (≤1), convert to %
            awk_res=$(awk -v r="$RATE" 'BEGIN { if (r <= 1.0) printf "%.6f", r*100; else printf "%.6f", r }')
            RATE=$awk_res
        fi

        # Check if this rate is the new minimum
        awk_check=$(awk -v r1="$RATE" -v r2="$MIN_RATE" 'BEGIN { if (r1 < r2) print 1; else print 0 }')
        if [[ "$awk_check" -eq 1 ]]; then
            MIN_RATE=$RATE
            MIN_M=$M
        fi
    done

    if [[ "$MIN_M" -ne -1 ]]; then
        printf "%-15s %-10s %-20s\n" "$BENCH" "$MIN_M" "$MIN_RATE"
    else
        printf "%-15s %-10s %-20s\n" "$BENCH" "N/A" "N/A"
    fi
done

echo "=============================================================="
