FLAGS = -Wall -Wextra -Werror -std=c11 # -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=unreachable
FILE = test1.txt test2.txt

all: s21_cat

s21_cat: s21_cat.c
	gcc $(FLAGS) -o $@ $^

clean:
	rm -f s21_cat

format:
	@find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -i

test: all
	clear
	bash test.sh
