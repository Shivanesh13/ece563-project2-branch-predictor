#!/usr/bin/env bash
# =============================================================
# Script: run_bimodal_sims.sh
# Purpose: Run bimodal predictor simulations for m = 7..20
#          using gcc_trace.txt, jpeg_trace.txt, and perl_trace.txt
# =============================================================

set -euo pipefail

# ---- Configuration ----
TRACES=("gcc_trace.txt" "jpeg_trace.txt" "perl_trace.txt")
START_M=7
END_M=20
OUT_DIR="results"

# Create output directory if not present
mkdir -p "$OUT_DIR"

# ---- Run Simulations ----
echo "=============================================="
echo "   Running Bimodal Predictor Simulations"
echo "=============================================="
echo ""

for TRACE in "${TRACES[@]}"; do
    BASENAME=$(basename "$TRACE" .txt)
    echo ">>> Benchmark: $BASENAME"
    
    for ((M=$START_M; M<=$END_M; M++)); do
        LOG_FILE="${OUT_DIR}/${BASENAME}_m${M}.log"
        echo "Running: ./sim bimodal ${M} ${TRACE}"
        
        # Run the simulation and redirect stdout & stderr
        ./sim bimodal "${M}" "${TRACE}" > "${LOG_FILE}" 2>&1
        
        echo "  ➜ Output saved to ${LOG_FILE}"
    done
    
    echo ""
done

echo "All simulations completed."
echo "Logs are stored in the '${OUT_DIR}' directory."
