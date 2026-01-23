#!/bin/bash
# MT25065_Part_C_shell.sh

CSV_FILE="MT25065_Part_C_CSV.csv"
CPU_CORE=0 


echo "Program+Function,CPU%,Mem,IO,Execution_Time_Seconds" > "$CSV_FILE"

run_experiment() {
    local label=$1
    local cmd=$2
    local worker=$3
    local prog_name=$(basename "$cmd")

    echo "Running Experiment: ${label}+${worker} on Core ${CPU_CORE}"


    START=$(date +%s.%N)
    
    # Pin to Core 0
    taskset -c $CPU_CORE $cmd $worker 2 &
    PID=$!

    # 1. Capture CPU/Mem
    # Takes 5 snapshots over 2.5 seconds to get a stable average
    METRICS=$(top -b -n 5 -d 0.5 -w 512 | awk -v name="$prog_name" '
        $0 ~ name { cpu_sum[it] += $9; mem_sum[it] += $6; active[it] = 1 }
        /top - / { it++ }
        END {
            total_cpu=0; total_mem=0; count=0;
            for (i=1; i<=it; i++) {
                if (active[i]) {
                    total_cpu += cpu_sum[i];
                    total_mem += mem_sum[i];
                    count++;
                }
            }
            if (count > 0) printf "%.2f,%.2f", total_cpu/count, total_mem/count;
            else printf "0.0,0.0";
        }')
    
    # 2. Capture IO
    IO_VAL=$(iostat -d 1 2 | awk 'BEGIN {sum=0; c=0} /^[a-zA-Z0-9]/ && !/Device/ {c++; if(c>1) sum += $3 + $4} END {print (sum ? sum : "0.0")}')

    # 3. Wait for program completion
    wait $PID
    
    END=$(date +%s.%N)
    DURATION=$(echo "$END - $START" | bc)

    echo "${label}+${worker},${METRICS},${IO_VAL},${DURATION}" >> "$CSV_FILE"
}

for worker in "cpu" "mem" "io"; do
    run_experiment "A" "./program_a" "$worker"
    run_experiment "B" "./program_b" "$worker"
done

echo "Done. Results saved to $CSV_FILE"