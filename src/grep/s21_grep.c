#define _GNU_SOURCE
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Flags {
  char *pattern;  // -e
  // -i, ignore uppercase vs. lowercase.
  int regexp_flags;
  int invert_match;          // -v, invert match.
  int count_matching_lines;  // -c, output count of matching lines only
  int list_files;            // -l, output matching files only.
  int lines_number;          // -n, precede each matching line with a line
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
void grep_file(FILE *file, Flags flags, regex_t *regex, char *filename);
char *concat(char *str1, const char *str2);

int main(int argc, char *argv[]) {
  Flags flags = read_flags(argc, argv);
  grep(argc, argv, flags);

  return 0;
}

Flags read_flags(int argc, char *argv[]) {
  Flags flags = {NULL, 0, 0, 0, 0, 0, 0, 0, 0};
  int opt;

  while ((opt = getopt_long(argc, argv, "e:ivcnsf:o", 0, 0)) != -1) {
    switch (opt) {
      case 'e':
        flags.regexp_flags |= REG_EXTENDED;
        flags.pattern = optarg;
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
        flags.lines_number = 1;
        break;
      case 's':
        flags.suppress_errors = 1;
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
  regex_t regex;
  int file_count = 0;

  // initial pattern search
  for (; pattern != end && pattern[0][0] == '-'; pattern++)
    ;

  if (pattern == end) {
    fprintf(stderr, "No pattern provided\n");
    exit(EXIT_FAILURE);
  }
  if (regcomp(&regex, *pattern, flags.regexp_flags)) {
    fprintf(stderr, "Failed to compile regular excretion");
    exit(EXIT_FAILURE);
  }

  for (char **filename = pattern + 1 /*&argv[1]*/; filename != end;
       filename++) {
    if (**filename == '-') continue;
    if (++file_count > 1) break;
  }

  for (char **filename = pattern + 1 /*&argv[1]*/; filename != end;
       filename++) {
    if (**filename == '-') continue;

    FILE *file = fopen(*filename, "rb");
    if (file == NULL) {
      if (!flags.suppress_errors) perror(*filename);
      continue;
    }

    if (file_count == 1) {
      grep_file(file, flags, &regex, "");
    } else {
      char *new_filename = concat(*filename, ":");

      grep_file(file, flags, &regex, new_filename);

      free(new_filename);
    }

    fclose(file);
  }

  regfree(&regex);
}

void grep_file(FILE *file, Flags flags, regex_t *regex, char *filename) {
  char *line = NULL;
  size_t length = 0;
  ssize_t read;
  regmatch_t match;
  int is_match = 0;
  int line_count = 0;
  int line_number = 0;
  // log(2147483647) + 1 digit + : + \0
  char *line_number_str = malloc(log10(INT_MAX) + 3);
  strcpy(line_number_str, "");

  if (flags.invert_match) is_match = REG_NOMATCH;

  if (flags.count_matching_lines) {
    while ((read = getline(&line, &length, file)) > 0) {
      if (regexec(regex, line, 1, &match, 0) == is_match) {
        line_count++;
      }
    }

    printf("%s%d\n", filename, line_count);
  } else if (!(flags.invert_match &&
               flags.output_only_matched_part)) {  // aka -vo, then do nothing
    while ((read = getline(&line, &length, file)) > 0) {
      if (flags.lines_number) {
        line_number++;
        sprintf(line_number_str, "%d:", line_number);
      }

      if (regexec(regex, line, 1, &match, 0) == is_match) {
        if (flags.output_only_matched_part) {
          printf("%s%s%.*s\n", filename, line_number_str,
                 match.rm_eo - match.rm_so, line + match.rm_so);
          char *remaining = line + match.rm_eo;
          while (!regexec(regex, remaining, 1, &match, 0)) {
            printf("%s%s%.*s\n", filename, line_number_str,
                   match.rm_eo - match.rm_so, remaining + match.rm_so);
            remaining += match.rm_eo;
          }
        } else
          printf("%s%s%s", filename, line_number_str, line);
      }
    }
  }

  free(line);
  free(line_number_str);
}

char *concat(char *str1, const char *str2) {
  char *result = malloc(strlen(str1) + strlen(str2) + 1);
  strcpy(result, str1);
  strcat(result, str2);

  return result;
}