flags=("-b" "--number-nonblank" "-E" "--show-ends" "-n" "--number" "-s" "--squeeze-blank" "-T" "--show-tabs" "-v" "--show-nonprinting")

for ((j=0; j<${#flags[@]}; j++))
do
    output=$(diff <(cat "${flags[$j]}" test1.txt) <(./s21_cat "${flags[$j]}" test1.txt))

    if [ -z "$output" ]; then
        echo "No differences for flag: ${flags[$j]}"
    else
        echo "Differences found for flag: ${flags[$j]}"
        echo "$output"
    fi
done