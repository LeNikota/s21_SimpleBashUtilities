FLAGS = -Wall -Wextra -Werror -std=c11 # -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=unreachable

all: s21_grep

s21_grep: s21_grep.c
	gcc $(FLAGS) -o $@ $^

clean:
	rm -f s21_grep

format:
	@find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -i

test: all
	bash test.sh
