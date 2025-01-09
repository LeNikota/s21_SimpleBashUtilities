FLAGS = -Wall -Wextra -Werror -std=c11 # -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=unreachable
FILE = test1.txt test2.txt
PATTERNS = pattern.txt
CHECK = @valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-leak-kinds=all

FLAGS = -Wall -Wextra -Werror -std=c11 # -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=unreachable
FILE = test1.txt test2.txt
CHECK = @valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-leak-kinds=all

all: s21_grep

s21_grep: s21_grep.c
	gcc $(FLAGS) -o $@ $^

clean:
	rm -f s21_grep

format:
	@find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -i

test:
	clear
	@echo "------------Diff------------"
	# ./s21_grep  this  $(FILE) > s21_grep.txt && grep  this  $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep  this  -n $(FILE) > s21_grep.txt && grep  this  -n $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep  this  -v $(FILE) > s21_grep.txt && grep  this  -v $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep  this  -nv $(FILE) > s21_grep.txt && grep  this  -nv $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep  this  -nvs $(FILE) lol.txt > s21_grep.txt && grep  this  -nvs $(FILE) lol.txt > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep  this  -o $(FILE) lol.txt > s21_grep.txt && grep  this  -o $(FILE) lol.txt > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep  this  -nso $(FILE) lol.txt > s21_grep.txt && grep  this  -nso $(FILE) lol.txt > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep  this  -l $(FILE) > s21_grep.txt && grep  this  -l $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep  this  -c $(FILE) > s21_grep.txt && grep  this  -c $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep -vo -c -f $(PATTERNS) $(FILE) > s21_grep.txt && grep -vo -c $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep -vol -c -e 3456 -f $(PATTERNS) $(FILE) > s21_grep.txt && grep -vol -c -e 3456 $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep -c -e 3456 -f $(PATTERNS) $(FILE) > s21_grep.txt && grep -c -e 3456 $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep -n -e 3456 -f $(PATTERNS) $(FILE) > s21_grep.txt && grep -n -e 3456 $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	# ./s21_grep -nso -e 3456 -f $(PATTERNS) $(FILE) > s21_grep.txt && grep -nso -e 3456 $(FILE) > grep.txt && diff s21_grep.txt grep.txt
	@rm -f s21_grep.txt grep.txt

	@echo "\n\n------------Static check------------"
	find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs clang-format -n
	find $(git rev-parse --show-toplevel) \( -name "*.c" -o -name "*.h" \) | xargs cppcheck --enable=all --suppress=missingIncludeSystem
	
	@echo "\n\n------------Memory leak check------------"
	$(CHECK) ./s21_grep this $(FILE) 2>&1 | grep -E "ERROR|leak"

	$(CHECK) ./s21_grep this -c $(FILE) 2>&1 | grep -E "ERROR|leak"
	$(CHECK) ./s21_grep this -l $(FILE) 2>&1 | grep -E "ERROR|leak"
	$(CHECK) ./s21_grep this -no $(FILE) 2>&1 | grep -E "ERROR|leak"
	$(CHECK) ./s21_grep this -vo $(FILE) 2>&1 | grep -E "ERROR|leak"
	$(CHECK) ./s21_grep this -vol $(FILE) 2>&1 | grep -E "ERROR|leak"
	
	$(CHECK) ./s21_grep -f $(PATTERNS)  $(FILE) 2>&1 | grep -E "ERROR|leak"
	$(CHECK) ./s21_grep -e this -e line -e 3456 $(FILE) 2>&1 | grep -E "ERROR|leak"
	$(CHECK) ./s21_grep -e this -e line -e 3456 -f $(PATTERNS) $(FILE) 2>&1 | grep -E "ERROR|leak"
	$(CHECK) ./s21_grep -vol -e this -e line -e 3456 -f $(PATTERNS) $(FILE) 2>&1 | grep -E "ERROR|leak"
