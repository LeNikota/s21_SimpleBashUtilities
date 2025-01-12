#!/bin/bash

flags=("-b" "-E" "-n" "-s" "-T" "-v" "--number-nonblank" "--show-ends" "--number" "--squeeze-blank" "--show-tabs" "--show-nonprinting")

if [[ ! -f test1.txt && ! -f test2.txt && ! -f test3.txt ]]; then
    git checkout HEAD^ test1.txt test2.txt test3.txt
fi

check_leaks() {
    local flags="$1"
    local files="$2"

    echo ./s21_cat $flags $files
    valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-leak-kinds=all ./s21_cat $flags $files 2>&1 | grep -E "ERROR"
}

check_diff() {
    local combination="$1"
    local files="$2"
    output=$(diff <(cat $combination $files) <(./s21_cat $combination $files))

    if [ -n "$output" ]; then
        echo "$combination $files"
        echo "Differences found for combination: $combination"
        echo "$output"
    fi
}

generate_combinations() {
    local prefix="$1"
    local start="$2"
    local end="$3"
    local arr=("${!4}")
    local files="$5"

    for ((i=start; i<end; i++)); do
        local new_combination="$prefix ${arr[i]}"
        check_diff "$new_combination" "$files"
        generate_combinations "$new_combination" $((i + 1)) $end "$4" "$files"
    done
}



echo "------------Diff------------"
generate_combinations "" 0 ${#flags[@]} flags[@] "test1.txt"
for ((j=1; j<${#flags[@]}; j++)); do
    generate_combinations "${flags[j]}" 0 ${#flags[@]} flags[@] "test1.txt"
done

generate_combinations "" 0 ${#flags[@]} flags[@] "test1.txt test2.txt"
for ((j=1; j<${#flags[@]}; j++)); do
    generate_combinations "${flags[j]}" 0 ${#flags[@]} flags[@] "test1.txt test2.txt"
done

generate_combinations "" 0 ${#flags[@]} flags[@] "test1.txt test2.txt test3.txt"
for ((j=1; j<${#flags[@]}; j++)); do
    generate_combinations "${flags[j]}" 0 ${#flags[@]} flags[@] "test1.txt test2.txt test3.txt"
done


echo
echo
echo
echo "------------Static check------------"
find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -n
find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs cppcheck --enable=all --suppress=missingIncludeSystem


echo
echo
echo
echo "------------Memory leak check------------"
check_leaks "test1.txt"
check_leaks -b "test1.txt"
check_leaks -n "test1.txt"
check_leaks -s "test1.txt"
check_leaks -bEnsT "test1.txt"
check_leaks -bEnsTv  "test1.txt"
check_leaks "test1.txt test2.txt"
check_leaks -b "test1.txt test2.txt"
check_leaks -n "test1.txt test2.txt"
check_leaks -s "test1.txt test2.txt"
check_leaks -bEnsT "test1.txt test2.txt"
check_leaks -bEnsTv  "test1.txt test2.txt"
check_leaks "test1.txt test2.txt test3.txt"
check_leaks -b "test1.txt test2.txt test3.txt"
check_leaks -n "test1.txt test2.txt test3.txt"
check_leaks -s "test1.txt test2.txt test3.txt"
check_leaks -bEnsT "test1.txt test2.txt test3.txt"
check_leaks -bEnsTv  "test1.txt test2.txt test3.txt"