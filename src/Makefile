CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=unreachable

# Source files
S21_CAT_SRC = ./cat/s21_cat.c
S21_GREP_SRC = ./grep/s21_grep.c

# Output binaries
S21_CAT_BIN = s21_cat
S21_GREP_BIN = s21_grep

# Default target
all: format $(S21_CAT_BIN) $(S21_GREP_BIN)


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


### Intensive remanence ###
# # Utilities
clean:
#   rm -rf *.o
# 	rm -rf ../build/*
#	rm -rf s21_cat s21_grep

# rebuild: clean all

format:
# 	clear
	find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -i
#! use this one	# find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs cppcheck --enable=all --suppress=missingIncludeSystem
# #	valgrind --tool=memcheck --leak-check=yes  ./build/graph


### Other ###
# ./s21_cat -nbsETv test.txt
# clear && flags=-n && files=test.txt && cat $flags $files > original.txt && make && ./s21_cat $files $flags > mine.txt && diff original.txt mine.txt