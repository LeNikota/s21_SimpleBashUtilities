FLAGS = -Wall -Wextra -Werror -std=c11 # -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=unreachable
FILE = test1.txt test2.txt
CHECK = @valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-leak-kinds=all

all: s21_cat

s21_cat: s21_cat.c
	gcc $(FLAGS) -o $@ $^

clean:
	rm -f s21_cat

format:
	@find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -i

test:
	clear
	@echo "------------Diff------------"
	# ./s21_cat $(FILE) > s21_cat.txt && cat $(FILE) > cat.txt && diff s21_cat.txt cat.txt
	# ./s21_cat -b $(FILE) > s21_cat.txt && cat -b $(FILE) > cat.txt && diff s21_cat.txt cat.txt
	# ./s21_cat -e $(FILE) > s21_cat.txt && cat -e $(FILE) > cat.txt && diff s21_cat.txt cat.txt
	# ./s21_cat -t $(FILE) > s21_cat.txt && cat -t $(FILE) > cat.txt && diff s21_cat.txt cat.txt
	# ./s21_cat -v $(FILE) > s21_cat.txt && cat -v $(FILE) > cat.txt && diff s21_cat.txt cat.txt
	# ./s21_cat -s $(FILE) > s21_cat.txt && cat -s $(FILE) > cat.txt && diff s21_cat.txt cat.txt
	# ./s21_cat -n $(FILE) > s21_cat.txt && cat -n $(FILE) > cat.txt && diff s21_cat.txt cat.txt
	# ./s21_cat -bnsTE $(FILE) > s21_cat.txt && cat -bnsTE $(FILE) > cat.txt && diff s21_cat.txt cat.txt
	# ./s21_cat -bnsTEv $(FILE) > s21_cat.txt && cat -bnsTEv $(FILE) > cat.txt && diff s21_cat.txt cat.txt
	@rm -f s21_cat.txt cat.txt

	@echo "\n\n------------Static check------------"
	find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -n
	find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs cppcheck --enable=all --suppress=missingIncludeSystem
	
	@echo "\n\n------------Memory leak check------------"
	$(CHECK) ./s21_cat -bEnsT test.txt 2>&1 | grep -E "ERROR|leak"
	$(CHECK) ./s21_cat -bEnsTv  test.txt 2>&1 | grep -E "ERROR|leak"
