#!/usr/bin/env bash

path_to_executable=../build/src/myPolygon
path_to_verify_script=../visualize/verify.py
path_to_custom_testcases=../custom-testcases


#build
cd ../build/ && cmake ../ && cmake --build . && cd ../test-scripts


for filename in ${path_to_custom_testcases}/*.in*; do
    echo "testing on ${filename}"

    ${path_to_executable} ${filename} ${filename/.in/.out} > /dev/null

    if [[ ${filename} == *".no-verify" ]]; then
        echo "  testcase too large, no-verify"
        continue
    fi

    python3 ${path_to_verify_script} ${filename} ${filename/.in/.out} 1000
    if [[ "$?" -ne 0 ]]; then
        echo "error occurs in ${filename}."
        set -o errexit
    fi
done