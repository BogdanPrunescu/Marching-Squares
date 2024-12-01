#!/bin/bash

# Array of input file names
input_files=("in_mic" "in_mediu" "in_mare")
output_files=("out_mic" "out_mediu" "out_mare")
thread_counts=(2 4 8 16)

# Loop over each input-output-thread combination
for i in "${!input_files[@]}"; do
  for threads in "${thread_counts[@]}"; do
    test_name="test_${input_files[$i]}_${threads}"
    echo "Running test: $test_name"
    vtune -collect hotspots -r "$test_name" ./ms_pthread "./checker/inputs/${input_files[$i]}.ppm" "./checker/outputs/${output_files[$i]}" "$threads"
    rm -rf ./checker/outputs/*
  done
done
