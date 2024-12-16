#!/bin/bash
input_files=("in_mic" "in_mediu" "in_mare")
output_files=("out_mic" "out_mediu" "out_mare")
thread_counts=(2 4 8)

for i in "${!input_files[@]}"; do
  for threads1 in "${thread_counts[@]}"; do
    for threads2 in "${thread_counts[@]}"; do
        sum=$((threads1 * threads2))
        test_name="test_${input_files[$i]}_${threads1}_${threads2}"
        
        if [ "$sum" -le 16 ]; then
            echo "Running test: $test_name"
            
            vtune -collect hotspots -r "$test_name" mpirun --oversubscribe -np "$threads1" ./ms_mpi_pthreads \
                "./checker/inputs/${input_files[$i]}.ppm" "./checker/outputs/${output_files[$i]}" "$threads2"
            
            rm -rf ./checker/outputs/*
        else
            echo "Skipping test: $test_name (sum of threads exceeds 16)"
        fi
    done
  done
done
