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
  int invert_match;          // -v, invert match.
  int count_matching_lines;  // -c, output count of matching lines only
  int list_files;            // -l, output matching files only.
  int number_line;           // -n, precede each matching line with a line
                             // number.
  int suppress_file_names;   // -h,output matching lines without preceding them
                             // by file names
  int suppress_errors;       // -s, suppress error messages about nonexistent or
                             // unreadable files.
  int output_only_matched_part;  // -o, Output the matched parts of a matching
                                 // line.
} Flags;

Flags read_flags(int argc, char *argv[]);
void grep(int argc, char *argv[], Flags flags);
void grep_file(FILE *file, char *filename, Flags flags, regex_t *preg);

int main(int argc, char *argv[]) {
  Flags flags = read_flags(argc, argv);
  grep(argc, argv, flags);

  return 0;
}

Flags read_flags(int argc, char *argv[]) {
  Flags flags = {0, 0, 0, 0, 0, 0, 0, 0};
  int opt;

  while ((opt = getopt(argc, argv, "eivco")) != -1) {
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
        flags.count_matching_lines = 1;
        break;
      case 'l':
        flags.list_files = 1;
        break;
      case 'n':
        flags.number_line = 1;
        break;
      case 'o':
        flags.output_only_matched_part = 1;
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

  for (char **filename = pattern + 1 /*&argv[1]*/; filename != end;
       filename++) {
    if (**filename == '-') continue;

    FILE *file = fopen(*filename, "rb");

    if (file == NULL) {
      fprintf(stderr, "%s ", argv[0]);
      perror(*filename);
      continue;
    }

    grep_file(file, *filename, flags, preg);

    fclose(file);
  }
}

void grep_file(FILE *file, char *filename, Flags flags, regex_t *preg) {
  char *line = NULL;
  size_t length = 0;
  ssize_t read;
  regmatch_t match;
  int is_match = 0;
  int line_count = 0;
  (void)filename;

  if (flags.invert_match) is_match = REG_NOMATCH;

  if (flags.count_matching_lines) {
    while ((read = getline(&line, &length, file)) > 0) {
      if (regexec(preg, line, 1, &match, 0) == is_match) {
        line_count++;
      }
    }
    printf("%d", line_count);
  } else {
    if (flags.invert_match && flags.output_only_matched_part)  // aka -vo
      return;
    while ((read = getline(&line, &length, file)) > 0) {
      if (regexec(preg, line, 1, &match, 0) == is_match) {
        if (flags.output_only_matched_part) {
          printf("%.*s\n", match.rm_eo - match.rm_so, line + match.rm_so);
          char *remaining = line + match.rm_eo;
          while (!regexec(preg, remaining, 1, &match, 0)) {
            printf("%.*s\n", match.rm_eo - match.rm_so,
                   remaining + match.rm_so);
            remaining += match.rm_eo;
          }
        } else
          printf("%s", line);
      }
    }
  }

  free(line);
}