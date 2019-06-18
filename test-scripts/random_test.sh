#!/usr/bin/env bash

err_file_num=0
path_to_executable=../build/src/myPolygon
path_to_randomcase_script=../visualize/random_case.py
path_to_verify_script=../visualize/verify.py
path_to_random_errorcases=../random-errorcases

for entry in ./* ; do
    if [[ ${entry} =~ ^\./errorcase\.in.* ]]; then
        ((err_file_num++))
    fi
done
echo "existed errorcase file: $err_file_num"

#build
cd ../build/ && cmake ../ && cmake --build . && cd ../test-scripts

#create folder for random errorcases
mkdir ${path_to_random_errorcases}


for i in $(seq 1 $1); do
    input_filename=random.in.${i}
    output_filename=random.out.${i}
    python3 ${path_to_randomcase_script}  ${input_filename} 1 3
    ${path_to_executable}  ${input_filename} ${output_filename} > /dev/null
    python3 ${path_to_verify_script} ${input_filename} ${output_filename} 1000
    if [[ "$?" -ne 0 ]]; then
        echo "error occurs in random.in.$i. restart with SV."
        set -o errexit
        sed -i 's/SO/SV/g' ${input_filename}
        ${path_to_executable}  ${input_filename} ${output_filename} > /dev/null
        python3 ${path_to_verify_script} ${input_filename} ${output_filename} 1000
        if [[ "$?" -ne 0 ]]; then
            echo "error occurs in random.in.$i. restart with SH"
            sed -i 's/SV/SH/g' ${input_filename}
            ${path_to_executable}  ${input_filename} ${output_filename} > /dev/null
            python3 ${path_to_verify_script} ${input_filename} ${output_filename} 1000
            if [[ "$?" -ne 0 ]]; then
                echo "error still occurs in random.in.$i. Abort!"
                sed -i 's/SH/SO/g' ${input_filename}
            else 
                echo "pass with SH!"
                sed -i 's/SH/SO/g' ${input_filename}
            fi
        else
            echo "pass with SV!"
            sed -i 's/SV/SO/g' ${input_filename}
        fi
        ((err_file_num++))
        cp ${input_filename} ${path_to_random_errorcases}/errorcase.in.${err_file_num}
        cp ${output_filename} ${path_to_random_errorcases}/errorcase.out.${err_file_num}
        rm ${input_filename} ${output_filename}
        break
    fi
    rm ${input_filename} ${output_filename}
done;
