#!/bin/bash

# Initialize a counter for differing files
wat_count=0
wasm_count=0
test_count=30

error_pass_count=0
error_fail_count=0
error_test_count=19

# Loop through all the regular test file pairs
for i in $(seq -w 01 $test_count); do
    # Set the file names
    code_file="test-${i}.tube"
    wat_file="test-${i}.wat"
    wasm_file="test-${i}.wasm"

    # Use Project3 to generate the WAT file for each test case.
    if [[ -f "../Project3" && -f "$code_file" ]]; then
        ../Project3 "$code_file" > "$wat_file"
    else
        echo "Executable ../Project3 or code file $code_file does not exist."
        continue
    fi

    if [ $? -ne 0 ]; then
        echo "Compilation of test $i to WAT format FAILED."
        rm -f "$wat_file"
        continue
    else
        ((wat_count++))
        echo "Compilation of test $i to WAT format SUCCESSFUL."
    fi

    # Double check that WAT file was generated; convert it to WASM
    if [[ -f "$wat_file" ]]; then
        wat2wasm "$wat_file"
    else
        echo "File '$wat_file' does not exist."
        continue
    fi

    if [ $? -ne 0 ]; then
        echo "                   ... to WASM format FAILED."
        rm -f "$wasm_file"
        continue
    else
        echo "                   ... to WASM format SUCCESSFUL."
    fi

    # Make sure WASM file was generated
    if [[ -f "$wasm_file" ]]; then
        ((wasm_count++))
    else
        echo "File '$wasm_file' does not exist."
    fi
done

echo ---
echo ERROR Testing

# Loop through all the ERROR test file pairs
for i in $(seq -w 01 $error_test_count); do
    # Set the file names
    code_file="test-error-${i}.tube"
    wat_file="output-error-${i}.wat"

    # Generate the output file for Project3
    if [[ -f "../Project3" && -f "$code_file" ]]; then
        ../Project3 "$code_file" > "$wat_file"
    else
        echo "Executable ../Project3 or code file $code_file does not exist."
        continue
    fi

    # Check the return code
    if [ $? -ne 0 ]; then
        echo "Error test $i ... Passed!"
        ((error_pass_count++))
    else
        echo "Error test $code_file failed (zero return code)."
        ((error_fail_count++))
    fi
done

# Report the final count of differing files
echo ---
echo "Of $test_count regular test files..."
echo "...generated $wat_count WAT files"
echo "...converted $wasm_count WAT files to wasm files for testing."
echo "Passed $error_pass_count of $error_test_count error tests (Failed $error_fail_count)"
