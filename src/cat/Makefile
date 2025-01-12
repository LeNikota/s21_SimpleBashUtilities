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

test: all
	clear
	@echo "------------Diff------------"
	bash test.sh

	@echo "\n\n------------Static check------------"
	find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -n
	find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs cppcheck --enable=all --suppress=missingIncludeSystem
	
	@echo "\n\n------------Memory leak check------------"
	$(CHECK) ./s21_cat test.txt 2>&1 | grep -E "ERROR"
	$(CHECK) ./s21_cat -bEnsT test.txt 2>&1 | grep -E "ERROR"
	$(CHECK) ./s21_cat -bEnsTv  test.txt 2>&1 | grep -E "ERROR"
