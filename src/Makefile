CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 # -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=unreachable

# Source files
S21_CAT_SRC = ./cat/s21_cat.c
S21_GREP_SRC = ./grep/s21_grep.c

# Output binaries
S21_CAT_BIN = s21_cat
S21_GREP_BIN = s21_grep

# Default target
all: $(S21_CAT_BIN) $(S21_GREP_BIN)
 	#!!! DELETE THAT AUTO_TEST
	./s21_cat --number
	./s21_cat --number --squeeze-blank
	./s21_cat --number-nonblank --number --squeeze-blank
	./s21_cat -n test.txt
	./s21_cat -b test.txt
	./s21_cat -nb test.txt
	./s21_cat -nbs test.txt
	./s21_cat -nbse test.txt
	./s21_cat -nbsE test.txt
	./s21_cat -nbst test.txt
	./s21_cat -nbsT  test.txt
	./s21_cat -nbsv test.txt


# Build s21_cat
$(S21_CAT_BIN): $(S21_CAT_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Build s21_grep
$(S21_GREP_BIN): $(S21_GREP_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up build files
clean:
	rm -f $(S21_CAT_BIN) $(S21_GREP_BIN)

# Phony targets
.PHONY: all clean


# # Utilities
# clean:
# 	rm -rf *.o
# 	rm -rf ../build/*

# rebuild: clean all

# format:
# 	clear
# 	find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -i
# 	find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs cppcheck --enable=all --suppress=missingIncludeSystem
# #	valgrind --tool=memcheck --leak-check=yes  ./build/graph
