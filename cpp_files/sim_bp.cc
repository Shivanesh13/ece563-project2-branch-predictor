#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_bp.h"
#include <unordered_map>
#include <iomanip>
#include <iostream>
using namespace std;

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr,addrb,addrg,addrk; // Variable holds the address read from input file

    unordered_map<unsigned long int, int> bimodel_table,gshare_table,chooser_table;

    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n %s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n %s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n %s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
    long int taken, not_taken;
    long int index_counter = 0;
    unsigned long int total_prediction, misprediction, branch_history;

    total_prediction = 0;
    misprediction = 0;
    branch_history = 0;
    taken = 1 << (params.N - 1);
    not_taken = ~taken;


    while(fscanf(FP, "%lx %s", &addr, str) != EOF){
        total_prediction++;
        outcome = str[0];
        addr = addr >> 2;
        if(strcmp(params.bp_name, "bimodal") == 0){
            addrb = addr & ((1 << params.M2) - 1);
            if(bimodel_table.find(addrb) == bimodel_table.end()){
                bimodel_table[addrb] = 2;
            }
            if((bimodel_table[addrb] < 2 && outcome == 't') || (bimodel_table[addrb] >= 2 && outcome == 'n')){
                    misprediction++;
            }
            if(outcome == 't' && bimodel_table[addrb] < 3){
                bimodel_table[addrb] += 1;
            } else if(outcome == 'n' && bimodel_table[addrb] > 0){
                bimodel_table[addrb] -= 1;
            }
        }


        if(strcmp(params.bp_name, "gshare") == 0) {
            addr = addr & ((1 << params.M1) - 1);
            addrg = (addr ^ (branch_history << (params.M1 - params.N)))& ((1 << params.M1) - 1);
            if(gshare_table.find(addrg) == gshare_table.end()){
                gshare_table[addrg] = 2;
            }
            if((gshare_table[addrg] < 2 && outcome == 't') || (gshare_table[addrg] >= 2 && outcome == 'n')){
                    misprediction++;
            }
            branch_history = branch_history >> 1;
            if(outcome == 't'){
                branch_history = branch_history | taken;
                if(gshare_table[addrg] < 3)
                    gshare_table[addrg] += 1;
            } else if(outcome == 'n'){
                branch_history = branch_history & not_taken;
                if(gshare_table[addrg] > 0)
                    gshare_table[addrg] -= 1;
            }
        }

        if(strcmp(params.bp_name, "hybrid") == 0) {
            addrk = addr & ((1 << params.K) - 1);
            addrb = addr & ((1 << params.M2) - 1);
            addr = addr & ((1 << params.M1) - 1);
            addrg = (addr ^ (branch_history << (params.M1 - params.N)))&& ((1 << params.M1) - 1);
            if(gshare_table.find(addrg) == gshare_table.end()){
                gshare_table[addrg] = 2;
            }
            if(bimodel_table.find(addrb) == bimodel_table.end()){
                bimodel_table[addrb] = 2;
            }
            if(chooser_table.find(addrk) == chooser_table.end()){
                chooser_table[addrk] = 1;
            }
            branch_history = branch_history >> 1;
            if(outcome == 't'){
                branch_history = branch_history | taken;
            } else if(outcome == 'n'){
                branch_history = branch_history & not_taken;
            }

            bool gshare_executed = (chooser_table[addrk] >= 2); 
            if(((outcome == 't' && gshare_table[addrg] >= 2) | (outcome == 'n' && gshare_table[addrg] < 2)) && ((outcome == 't' && bimodel_table[addrb] < 2) | (outcome == 'n' && bimodel_table[addrb] >= 2))){
                if(chooser_table[addrk] < 3){
                    chooser_table[addrk] += 1;
                }
            } else if(((outcome == 't' && gshare_table[addrg] < 2) | (outcome == 'n' && gshare_table[addrg] >= 2)) && ((outcome == 't' && bimodel_table[addrb] >= 2) | (outcome == 'n' && bimodel_table[addrb] < 2))){
                if(chooser_table[addrk] > 0){
                    chooser_table[addrk] -= 1;
                }
            }

            if(gshare_executed){
                if((outcome == 't' && gshare_table[addrg] < 2) || (outcome == 'n' && gshare_table[addrg] >= 2)){
                    misprediction++;
                }
                if((outcome == 't' && gshare_table[addrg] < 3)){
                    gshare_table[addrg] += 1;
                } else if((outcome == 'n' && gshare_table[addrg] > 0)){
                    gshare_table[addrg] -= 1;
                }

            } 
            else {
                if((outcome == 't' && bimodel_table[addrb] < 2) || (outcome == 'n' && bimodel_table[addrb] >= 2)){
                    misprediction++;
                }
                if(outcome == 't' && bimodel_table[addrb] < 3){
                    bimodel_table[addrb] += 1;
                } else if(outcome == 'n' && bimodel_table[addrb] > 0){
                    bimodel_table[addrb] -= 1;
                }
            }
        }
        

    }

        printf("OUTPUT\n");
        printf(" number of predictions:    %lu\n", total_prediction);
        printf(" number of mispredictions: %lu\n", misprediction);
        printf(" misprediction rate:       %.2f%%\n", ((double)misprediction / (double)total_prediction) * 100);

    if(strcmp(params.bp_name, "bimodal") == 0){
        printf("FINAL BIMODAL CONTENTS\n");

        for (unsigned long int i = 0; i < (1 << params.M2); i++) {
            if(bimodel_table.find(i) == bimodel_table.end())
                cout << " " << i << "\t" << "2" << "\n";   
            else    
                cout << " " << i << "  " << bimodel_table[i] << "\n";
        }
    }
    else if(strcmp(params.bp_name, "gshare") == 0){
        printf("FINAL GSHARE CONTENTS\n");

        for (unsigned long int i = 0; i < (1 << params.M1); i++) {
            if(gshare_table.find(i) == gshare_table.end()){
                cout << " " << i << "\t" << "2" << "\n";
            } else 
                cout << " " << i <<  "\t" << gshare_table[i] << "\n";
        }
    } else {
        printf("FINAL CHOOSER CONTENTS\n");
        for (unsigned long int i = 0; i < (1 << params.K); i++) {
            if(chooser_table.find(i) == chooser_table.end()){
                cout << " " << i << "\t" << "2" << "\n";
            } else 
                cout << " " << i <<  "\t" << chooser_table[i] << "\n";
        }
        printf("FINAL GSHARE CONTENTS\n");

        for (unsigned long int i = 0; i < (1 << params.M1); i++) {
            if(gshare_table.find(i) == gshare_table.end()){
                cout << " " << i << "\t" << "2" << "\n";
            } else 
                cout << " " << i <<  "\t" << gshare_table[i] << "\n";
        }


        printf("FINAL BIMODAL CONTENTS\n");

        for (unsigned long int i = 0; i < (1 << params.M2); i++) {
            if(bimodel_table.find(i) == bimodel_table.end())
                cout << " " << i << "\t" << "2" << "\n";   
            else    
                cout << " " << i << "  " << bimodel_table[i] << "\n";
        }

        
    }

    return 0;
}
