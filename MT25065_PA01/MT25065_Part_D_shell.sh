#!/bin/bash
# MT25065_Part_D_shell.sh

CSV_FILE="MT25065_Part_D_CSV.csv"
CPU_CORE=0

echo "Program,Worker,Count,CPU_Percent,Mem,IO,Time_Seconds" > "$CSV_FILE"

run_scaling() {
    local prog_label=$1   # A or B
    local cmd=$2          # program_a or program_b
    local worker=$3       # cpu, mem, or io
    local count=$4        # Number of processes/threads
    local prog_name=$(basename "$cmd")

    echo "Running Scaling: Program ${prog_label} | Worker ${worker} | Count ${count} | Core ${CPU_CORE}"

    # Record Start Time
    START=$(date +%s.%N)
    
    # Pin to Core 0 and run in background
    taskset -c $CPU_CORE $cmd $worker $count &
    PID=$!

    # Capture CPU & Mem
    #  5 samples, 1 second apart (
    METRICS=$(top -b -n 5 -d 1 -w 512 | awk -v name="$prog_name" '
        $0 ~ name { cpu_sum[it] += $9; mem_sum[it] += $6; active[it] = 1 }
        /top - / { it++ }
        END {
            t_cpu=0; t_mem=0; c=0;
            for (i=1; i<=it; i++) { 
                if (active[i]) { 
                    t_cpu += cpu_sum[i]; 
                    t_mem += mem_sum[i]; 
                    c++; 
                } 
            }
            if (c > 0) printf "%.2f,%.2f", t_cpu/c, t_mem/c;
            else printf "0.0,0.0";
        }')
    
    # 2. Capture IO (Disk Throughpu)
    # samples of 1 second each
    IO_VAL=$(iostat -d 1 2 | awk 'BEGIN {sum=0; c=0} /^[a-zA-Z0-9]/ && !/Device/ {c++; if(c>1) sum += $3 + $4} END {print (sum ? sum : "0.0")}')

    wait $PID
    
    END=$(date +%s.%N)
    DURATION=$(echo "$END - $START" | bc)

    echo "${prog_label},${worker},${count},${METRICS},${IO_VAL},${DURATION}" >> "$CSV_FILE"
}
# Part D Scaling Program A 
for worker in "cpu" "mem" "io"; do
    for c in {2..5}; do 
        run_scaling "A" "./program_a" "$worker" "$c"
    done
done
# Part D Scaling Program B 
for worker in "cpu" "mem" "io"; do
    for c in {2..8}; do 
        run_scaling "B" "./program_b" "$worker" "$c"
    done
done

echo "Part D Scaling Experiments Completed. Data saved to $CSV_FILE"