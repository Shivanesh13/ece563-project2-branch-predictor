# ECE 563 Project 2: Branch Predictor Simulator

## Overview
This project implements a comprehensive branch predictor simulator that evaluates different branch prediction strategies including bimodal, gshare, and hybrid predictors. The simulator analyzes branch prediction accuracy across various benchmark traces and provides detailed performance metrics.

## Features
- **Multiple Predictor Types**: Bimodal, Gshare, and Hybrid branch predictors
- **Configurable Parameters**: Adjustable predictor table sizes and history lengths
- **Performance Analysis**: Detailed prediction accuracy statistics
- **Trace-based Simulation**: Real benchmark trace analysis
- **Hybrid Predictor**: Meta-predictor combining multiple prediction strategies

## Project Structure
```
Project_2/
├── cpp_files/            # Main implementation
│   ├── Makefile
│   ├── sim_bp.cc/h       # Branch predictor simulator
│   ├── sim               # Compiled executable
│   ├── *.txt             # Test outputs and results
│   ├── results/          # Detailed simulation logs
│   │   ├── *_trace_m*.log  # Results for different configurations
│   └── scripts/          # Automation scripts
├── proj2-traces/         # Benchmark trace files
│   ├── gcc_trace.txt
│   ├── jpeg_trace.txt
│   └── perl_trace.txt
└── *.zip                 # Project archives
```

## Predictor Types

### Bimodal Predictor
- Uses program counter (PC) to index prediction table
- Simple 2-bit saturating counters
- Good for loops and regular patterns

### Gshare Predictor
- Combines PC with global branch history
- XORs PC with global history register
- Better for complex branch patterns

### Hybrid Predictor
- Meta-predictor that chooses between bimodal and gshare
- Uses additional predictor to select best strategy
- Adapts to different code regions

## Build Instructions
```bash
cd cpp_files
make clean
make
```

## Usage

### Command Line Format

#### Bimodal Predictor
```bash
./sim bimodal <table_size> <trace_file>
```

#### Gshare Predictor
```bash
./sim gshare <table_size> <history_length> <trace_file>
```

#### Hybrid Predictor
```bash
./sim hybrid <bimodal_table_size> <gshare_table_size> <history_length> <meta_table_size> <trace_file>
```

### Parameters
- `table_size`: Number of entries in prediction table (power of 2)
- `history_length`: Length of global history register (for gshare/hybrid)
- `meta_table_size`: Size of meta-predictor table (for hybrid)
- `trace_file`: Path to branch trace file

### Example Usage
```bash
# Bimodal predictor with 2048 entries
./sim bimodal 11 gcc_trace.txt

# Gshare predictor with 2048 entries, 10-bit history
./sim gshare 11 10 gcc_trace.txt

# Hybrid predictor: 1024 bimodal + 1024 gshare entries, 8-bit history, 512 meta entries
./sim hybrid 10 10 8 9 gcc_trace.txt

# View output with less
./sim bimodal 11 gcc_trace.txt | less
```

## Trace Files
- `gcc_trace.txt`: GCC compilation branch traces
- `jpeg_trace.txt`: JPEG compression benchmark traces
- `perl_trace.txt`: Perl interpreter execution traces

## Results and Analysis

### Prediction Accuracy
The simulator provides detailed accuracy metrics:
- Overall prediction accuracy percentage
- Number of correct/incorrect predictions
- Branch type analysis

### Configuration Analysis
Extensive testing across different configurations shows:
- Gshare generally outperforms bimodal for complex applications
- Hybrid predictors can adapt to different code patterns
- Table size vs accuracy trade-offs

### Benchmark Results
The `results/` directory contains comprehensive simulation data for various predictor configurations across all benchmarks, showing prediction accuracy ranging from ~85-95% depending on the application and configuration.

## Validation
The project includes validation files (`val_*.txt`) that verify simulator correctness against expected outputs for different predictor configurations and trace files.

## Key Findings
1. **Predictor Selection**: Choice of predictor depends on application characteristics
2. **Table Size Impact**: Larger tables improve accuracy but with diminishing returns
3. **History Length**: Optimal history length varies by application
4. **Hybrid Advantage**: Hybrid predictors can achieve better average performance across diverse workloads

## Dependencies
- C++ compiler (g++ recommended)
- Make build system

## Output Format
The simulator outputs:
- Predictor configuration summary
- Prediction statistics (correct, incorrect, total branches)
- Overall prediction accuracy percentage
- Detailed breakdown by branch types

## Academic Context
This project is part of ECE 563: Computer Architecture, demonstrating branch prediction techniques used in modern processors. The simulator illustrates fundamental concepts in dynamic branch prediction, history-based prediction, and adaptive prediction strategies.
