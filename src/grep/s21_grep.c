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
  int invert_match;         // -v, invert match.
  int matched_lines_count;  // -c, output count of matching lines only
  int list_file_names;      // -l, output matching files name only.
  int lines_number;         // -n, precede each matching line with a line
                            // number.
  int suppress_file_names;  // -h,output matching lines without preceding them
                            // by file names
  int suppress_errors;      // -s, suppress error messages about nonexistent or
                            // unreadable files.
  int output_matched_part;  // -o, Output the matched parts of a matching
                            // line.
} Flags;

Flags read_flags(int argc, char *argv[]);
void grep(int argc, char *argv[], Flags flags);
void output_matched_line(FILE *file, Flags flags, regex_t *regex,
                         char *filename);
void output_matched_part(FILE *file, Flags flags, regex_t *regex,
                         char *filename);
void output_matched_lines_count(FILE *file, Flags flags, regex_t *regex,
                                char *filename);
void list_file_names(FILE *file, Flags flags, regex_t *regex, char *filename);
char *concat(char *str1, const char *str2);

int main(int argc, char *argv[]) {
  Flags flags = read_flags(argc, argv);

  grep(argc, argv, flags);

  return 0;
}

Flags read_flags(int argc, char *argv[]) {
  Flags flags = {NULL, 0, 0, 0, 0, 0, 0, 0, 0};
  int opt;

  while ((opt = getopt(argc, argv, "e:ivcnshlf:o")) != -1) {
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
        flags.matched_lines_count = 1;
        break;
      case 'l':
        flags.list_file_names = 1;
        break;
      case 'n':
        flags.lines_number = 1;
        break;
      case 's':
        flags.suppress_errors = 1;
        break;
      case 'h':
        flags.suppress_file_names = 1;
        break;
      case 'o':
        flags.output_matched_part = 1;
        break;
      case '?':
        fprintf(stderr, "Invalid option: %s\n", argv[optind - 1]);
        exit(EXIT_FAILURE);
        break;
    }
  }

  // grep [OPTION...] ->PATTERNS<- [FILE...]
  if (flags.pattern == NULL) {
    flags.pattern = argv[optind];
    optind++;
  }

  return flags;
}

void grep(int argc, char *argv[], Flags flags) {
  // aka -vo, then do nothing
  if (flags.output_matched_part && flags.invert_match)
    // aka -vol, then continue
    if (!flags.list_file_names) return;

  char **pattern = &argv[1];
  char **end = &argv[argc];
  regex_t regex;
  int file_count = 0;

  if (flags.pattern == NULL) {
    fprintf(stderr, "No pattern provided\n");
    exit(EXIT_FAILURE);
  }
  if (regcomp(&regex, flags.pattern, flags.regexp_flags)) {
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
      if (!(flags.suppress_errors || flags.list_file_names)) perror(*filename);
      continue;
    }

    char *new_filename = concat("", "");
    if (file_count > 1 && !flags.suppress_file_names) {
      new_filename = concat(*filename, ":");
    }

    if (flags.list_file_names) {
      list_file_names(file, flags, &regex, *filename);
    } else if (flags.output_matched_part) {
      output_matched_part(file, flags, &regex, new_filename);
    } else if (flags.matched_lines_count) {
      output_matched_lines_count(file, flags, &regex, new_filename);
    } else {
      output_matched_line(file, flags, &regex, new_filename);
    }

    fclose(file);
    free(new_filename);
  }

  regfree(&regex);
}

void output_matched_line(FILE *file, Flags flags, regex_t *regex,
                         char *filename) {
  char *line = NULL;
  size_t length = 0;
  ssize_t read;
  regmatch_t match;
  int is_match = 0;
  int line_number = 0;
  // log(2147483647) + 1 digit + : + \0
  char *line_number_str = malloc(log10(INT_MAX) + 3);
  strcpy(line_number_str, "");

  if (flags.invert_match) is_match = REG_NOMATCH;

  while ((read = getline(&line, &length, file)) > 0) {
    if (flags.lines_number) {
      line_number++;
      sprintf(line_number_str, "%d:", line_number);
    }

    if (regexec(regex, line, 1, &match, 0) == is_match)
      printf("%s%s%s", filename, line_number_str, line);
  }

  free(line);
  free(line_number_str);
}
void output_matched_part(FILE *file, Flags flags, regex_t *regex,
                         char *filename) {
  char *line = NULL;
  size_t length = 0;
  ssize_t read;
  regmatch_t match;
  int line_number = 0;
  // For line numbering, log(2147483647) + 1 digit + : + \0
  char *line_number_str = malloc(log10(INT_MAX) + 3);
  strcpy(line_number_str, "");

  while ((read = getline(&line, &length, file)) > 0) {
    if (flags.lines_number) {
      line_number++;
      sprintf(line_number_str, "%d:", line_number);
    }

    if (!regexec(regex, line, 1, &match, 0)) {
      printf("%s%s%.*s\n", filename, line_number_str, match.rm_eo - match.rm_so,
             line + match.rm_so);

      char *remaining = line + match.rm_eo;

      while (!regexec(regex, remaining, 1, &match, 0)) {
        printf("%s%s%.*s\n", filename, line_number_str,
               match.rm_eo - match.rm_so, remaining + match.rm_so);

        remaining += match.rm_eo;
      }
    }
  }

  free(line);
  free(line_number_str);
}
void output_matched_lines_count(FILE *file, Flags flags, regex_t *regex,
                                char *filename) {
  char *line = NULL;
  size_t length = 0;
  ssize_t read;
  regmatch_t match;
  int is_match = 0;
  int line_count = 0;

  if (flags.invert_match) is_match = REG_NOMATCH;

  while ((read = getline(&line, &length, file)) > 0) {
    if (regexec(regex, line, 1, &match, 0) == is_match) {
      line_count++;
    }
  }

  printf("%s%d\n", filename, line_count);

  free(line);
}
void list_file_names(FILE *file, Flags flags, regex_t *regex, char *filename) {
  char *line = NULL;
  size_t length = 0;
  ssize_t read;
  regmatch_t match;
  int is_match = 0;

  if (flags.invert_match) is_match = REG_NOMATCH;

  while ((read = getline(&line, &length, file)) > 0) {
    if (regexec(regex, line, 1, &match, 0) == is_match) {
      printf("%s\n", filename);
      break;
    }
  }

  free(line);
}

char *concat(char *str1, const char *str2) {
  char *result = malloc(strlen(str1) + strlen(str2) + 1);

  if (result == NULL) {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  strcpy(result, str1);
  strcat(result, str2);

  return result;
}