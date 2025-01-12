#!/bin/bash

flags=("-e this" "-ewow" "-e 345" "-i" "-v" "-c" "-l" "-n" "-h" "-s" "-f pattern.txt" "-o")
flags_without_ef=("this" "-i" "-v" "-c" "-l" "-n" "-h" "-s" "-o")

if [[ ! -f test1.txt && ! -f test2.txt && ! -f test3.txt && ! -f pattern.txt ]]; then
    git checkout HEAD^ test1.txt test2.txt test3.txt pattern.txt
fi


check_leaks() {
    local input="$1"

    echo ./s21_grep $input
    valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-leak-kinds=all ./s21_grep $input 2>&1 | grep -E "ERROR"
}

check_diff() {
    local combination="$1"
    local files="$2"

    # echo "$combination $files"
    output=$(diff <(grep $combination $files) <(./s21_grep $combination $files))

    if [ -n "$output" ]; then
        echo "Differences found for combination: $combination $files"
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



# echo "------------Diff------------"
# generate_combinations "" 0 ${#flags[@]} flags[@] "test1.txt"
# generate_combinations "" 0 ${#flags_without_ef[@]} flags_without_ef[@] "test1.txt"
# for ((j=1; j<${#flags[@]}; j++)); do
#     generate_combinations "${flags[j]}" 0 ${#flags[@]} flags[@] "test1.txt"
#     generate_combinations "" 0 ${#flags_without_ef[@]} flags_without_ef[@] "test1.txt"
# done

# generate_combinations "" 0 ${#flags[@]} flags[@] "test1.txt test2.txt"
# generate_combinations "" 0 ${#flags_without_ef[@]} flags_without_ef[@] "test1.txt test2.txt"
# for ((j=1; j<${#flags[@]}; j++)); do
#     generate_combinations "${flags[j]}" 0 ${#flags[@]} flags[@] "test1.txt test2.txt"
#     generate_combinations "" 0 ${#flags_without_ef[@]} flags_without_ef[@] "test1.txt test2.txt"
# done

# generate_combinations "" 0 ${#flags[@]} flags[@] "test1.txt test2.txt test3.txt"
# generate_combinations "" 0 ${#flags_without_ef[@]} flags_without_ef[@] "test1.txt test2.txt test3.txt"
# for ((j=1; j<${#flags[@]}; j++)); do
#     generate_combinations "${flags[j]}" 0 ${#flags[@]} flags[@] "test1.txt test2.txt test3.txt"
#     generate_combinations "" 0 ${#flags_without_ef[@]} flags_without_ef[@] "test1.txt test2.txt test3.txt"
# done




# echo
# echo
# echo
# echo "------------Static check------------"
# find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -n
# find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs cppcheck --enable=all --suppress=missingIncludeSystem


echo
echo
echo
echo "------------Memory leak check------------"
check_leaks "this test1.txt"
check_leaks "this -c test1.txt"
check_leaks "this -cv test1.txt"
check_leaks "this -l test1.txt"
check_leaks "this -lv test1.txt"
check_leaks "this -no test1.txt"
check_leaks "this -vo test1.txt"
check_leaks "this -vol test1.txt"
check_leaks "-f pattern.txt  test1.txt"
check_leaks "-e this -e line -e 3456 test1.txt"
check_leaks "-e this -e line -e 3456 -f pattern.txt test1.txt"
check_leaks "-c -e this -e line -e 3456 -f pattern.txt test1.txt"
check_leaks "-v -e this -e line -e 3456 -f pattern.txt test1.txt"
check_leaks "-vo -e this -e line -e 3456 -f pattern.txt test1.txt"
check_leaks "-vol -e this -e line -e 3456 -f pattern.txt test1.txt"
check_leaks "-vocn -e this -e line -e 3456 -f pattern.txt test1.txt"
check_leaks "-vocnl -e this -e line -e 3456 -f pattern.txt test1.txt"


check_leaks "this test1.txt test2.txt"
check_leaks "this -c test1.txt test2.txt"
check_leaks "this -cv test1.txt test2.txt"
check_leaks "this -l test1.txt test2.txt"
check_leaks "this -lv test1.txt test2.txt"
check_leaks "this -no test1.txt test2.txt"
check_leaks "this -vo test1.txt test2.txt"
check_leaks "this -vol test1.txt test2.txt"
check_leaks "-f pattern.txt  test1.txt test2.txt"
check_leaks "-e this -e line -e 3456 test1.txt test2.txt"
check_leaks "-e this -e line -e 3456 -f pattern.txt test1.txt test2.txt"
check_leaks "-c -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt"
check_leaks "-v -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt"
check_leaks "-vo -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt"
check_leaks "-vol -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt"
check_leaks "-vocn -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt"
check_leaks "-vocnl -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt"

check_leaks "this test1.txt test2.txt test3.txt"
check_leaks "this -c test1.txt test2.txt test3.txt"
check_leaks "this -cv test1.txt test2.txt test3.txt"
check_leaks "this -l test1.txt test2.txt test3.txt"
check_leaks "this -lv test1.txt test2.txt test3.txt"
check_leaks "this -no test1.txt test2.txt test3.txt"
check_leaks "this -vo test1.txt test2.txt test3.txt"
check_leaks "this -vol test1.txt test2.txt test3.txt"
check_leaks "-f pattern.txt  test1.txt test2.txt test3.txt"
check_leaks "-e this -e line -e 3456 test1.txt test2.txt test3.txt"
check_leaks "-e this -e line -e 3456 -f pattern.txt test1.txt test2.txt test3.txt"
check_leaks "-c -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt test3.txt"
check_leaks "-v -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt test3.txt"
check_leaks "-vo -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt test3.txt"
check_leaks "-vol -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt test3.txt"
check_leaks "-vocn -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt test3.txt"
check_leaks "-vocnl -e this -e line -e 3456 -f pattern.txt test1.txt test2.txt test3.txt"