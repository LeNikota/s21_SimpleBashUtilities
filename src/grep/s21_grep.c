#define _GNU_SOURCE
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Flags {
  // -e, POSIX Extended Regular Expressions
  // -i, ignore uppercase vs. lowercase.
  int regexp_flags;
  int invert_match;         // -v, invert match.
  int count_only_matching;  // -c, output count of matching lines only
  int list_files;           // -l, output matching files only.
  int number_line;          // -n, precede each matching line with a line
                            // number.
  int suppress_file_names;  // -h,output matching lines without preceding them
                            // by file names
  int suppress_errors;      // -s, suppress error messages about nonexistent or
                            // unreadable files.
  int output_only_matches;  // -o, output count of matching lines only
  // char *pattern;         // -e Pattern to search for
} Flags;

Flags read_flags(int argc, char *argv[]);
void grep(int argc, char *argv[], Flags flags);
void grep_file(FILE *file, Flags flags, regex_t *preg);

int main(int argc, char *argv[]) {
  Flags flags = read_flags(argc, argv);
  grep(argc, argv, flags);

  return 0;
}

Flags read_flags(int argc, char *argv[]) {
  Flags flags = {0, 0, 0, 0, 0, 0, 0, 0};
  int opt;

  while ((opt = getopt(argc, argv, "eiv")) != -1) {
    switch (opt) {
      case 'e':
        flags.regexp_flags |= REG_EXTENDED;
        break;
      case 'i':
        flags.regexp_flags |= REG_ICASE;
        break;
      case 'v':
        flags.invert_match = 1;
        break;
      case 'c':
        flags.count_only_matching = 1;
        break;
      case 'l':
        flags.list_files = 1;
        break;
      case 'n':
        flags.number_line = 1;
        break;
      case '?':
        fprintf(stderr, "Invalid option: %s\n", argv[optind - 1]);
        exit(EXIT_FAILURE);
        break;
    }
  }
  return flags;
}

void grep(int argc, char *argv[], Flags flags) {
  char **pattern = &argv[1];
  char **end = &argv[argc];
  regex_t preg_storage;
  regex_t *preg = &preg_storage;

  for (; pattern != end && pattern[0][0] == '-'; pattern++)
    ;

  if (pattern == end) {
    fprintf(stderr, "No pattern provided\n");
    exit(EXIT_FAILURE);
  }
  if (regcomp(preg, *pattern, flags.regexp_flags)) {
    fprintf(stderr, "Failed to compile regular excretion");
    exit(EXIT_FAILURE);
  }

  for (char **filename = /*pattern + 1*/ &argv[1]; filename != end;
       filename++) {
    if (**filename == '-') continue;

    FILE *file = fopen(*filename, "rb");

    if (file == NULL) {
      fprintf(stderr, "%s ", argv[0]);
      perror(*filename);
      continue;
    }

    grep_file(file, flags, preg);

    fclose(file);
  }
}

void grep_file(FILE *file, Flags flags, regex_t *preg) {
  char *line = NULL;
  size_t length = 0;
  ssize_t read;
  regmatch_t match;
  int is_match = 0;

  if (flags.invert_match) is_match = REG_NOMATCH;

  while ((read = getline(&line, &length, file)) > 0) {
    if (regexec(preg, line, 1, &match, 0) == is_match) {
      printf("%s", line);
    }
  }

  free(line);
}