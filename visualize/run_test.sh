err_file_num=0

for entry in ./* ; do
    echo $entry
    if [[ $entry =~ ^\./errorcase\.in.* ]]; then
        ((err_file_num++))
    fi
done
echo $err_file_num
for i in {1..100}; do
    python random_case.py random.in.$i $i 5
    ../build/myPolygon  random.in.$i random.out.$i
    python verify.py random.in.$i random.out.$i 1000
    if [ "$?" -ne 0 ]; then
        echo "error occurs in random.in.$i. Abort!"
        ((err_file_num++))
        cp random.in.$i errorcase.in.$err_file_num
        cp random.out.$i errorcase.out.$err_file_num
        rm random.in.$i random.out.$i
        break
    fi
    rm random.in.$i random.out.$i
    # read -p "Press enter to continue"
done;
