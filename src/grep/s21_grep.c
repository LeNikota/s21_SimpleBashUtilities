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
  int suppress_file_names;  // -h, output matching lines without preceding them
                            // by file names
  int suppress_errors;      // -s, suppress error messages about nonexistent or
                            // unreadable files.
  int pattern_from_file;    // -f, file	Take regexes from a file.
  int output_matched_part;  // -o, Output the matched parts of a matching
  // line.

  int many_files;  // To prefix output with filename when more than 1
} Flags;

Flags read_flags(int argc, char *argv[]);
void add_pattern(Flags *flags, char *pattern);
void add_pattern_from_file(Flags *flags, char *filename);
void count_entered_files(int argc, char *argv[], Flags *flags);
void grep(int argc, char *argv[], Flags flags);
void process_file(FILE *file, Flags flags, regex_t *regex, char *filename);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t new_size);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: grep [OPTION]... PATTERNS [FILE]...\n");
    exit(EXIT_FAILURE);
  }

  Flags flags = read_flags(argc, argv);
  count_entered_files(argc, argv, &flags);
  grep(argc, argv, flags);

  free(flags.pattern);
  return 0;
}

Flags read_flags(int argc, char *argv[]) {
  Flags flags = {0};
  int opt;

  flags.regexp_flags |= REG_EXTENDED;

  while ((opt = getopt(argc, argv, "e:ivcnshlf:o")) != -1) {
    switch (opt) {
      case 'e':
        add_pattern(&flags, optarg);
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
      case 'h':
        flags.suppress_file_names = 1;
        break;
      case 's':
        flags.suppress_errors = 1;
        break;
      case 'f':
        flags.pattern_from_file = 1;
        add_pattern_from_file(&flags, optarg);
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
  if (flags.pattern == NULL && !flags.pattern_from_file) {
    add_pattern(&flags, argv[optind]);
    optind++;
  }

  return flags;
}

void count_entered_files(int argc, char *argv[], Flags *flags) {
  char **start = argv + optind;
  char **end = argv + argc;
  int count = 0;

  for (char **filename = start; filename != end; filename++) {
    count++;

    if (count > 1) {
      flags->many_files = 1;
      break;
    }
  }
}

void grep(int argc, char *argv[], Flags flags) {
  // aka -vo, then do nothing
  if (flags.output_matched_part && flags.invert_match)
    // aka -vol or -voc, then continue
    if (!(flags.list_file_names || flags.matched_lines_count)) return;

  regex_t regex;
  char **start = argv + optind;
  char **end = argv + argc;

  if (regcomp(&regex, flags.pattern, flags.regexp_flags)) {
    fprintf(stderr, "Failed to compile regular excretion");
    exit(EXIT_FAILURE);
  }

  for (char **filename = start; filename != end; filename++) {
    FILE *file = fopen(*filename, "rb");
    if (file == NULL) {
      if (!flags.suppress_errors) perror(*filename);
      continue;
    }

    process_file(file, flags, &regex, *filename);

    fclose(file);
  }

  regfree(&regex);
}

void process_file(FILE *file, Flags flags, regex_t *regex, char *filename) {
  char *line = NULL;
  size_t length = 0;
  ssize_t read;
  regmatch_t match;
  int line_number = 0;
  int line_count = 0;
  int is_match = 0;

  if (flags.invert_match) is_match = REG_NOMATCH;

  if (flags.list_file_names) {
    // List filenames with at least one match line
    while ((read = getline(&line, &length, file)) > 0) {
      if (regexec(regex, line, 1, &match, 0) == is_match) {
        printf("%s\n", filename);
        break;
      }
    }

  } else if (flags.matched_lines_count) {
    // List count of matched lines
    while ((read = getline(&line, &length, file)) > 0) {
      if (regexec(regex, line, 1, &match, 0) == is_match) {
        line_count++;
      }
    }
    if (flags.many_files && !flags.suppress_file_names) {
      printf("%s:", filename);
    }
    printf("%d\n", line_count);
  } else if (flags.output_matched_part) {
    // List matched parts
    while ((read = getline(&line, &length, file)) > 0) {
      line_number++;

      char *remaining = line;

      while (!regexec(regex, remaining, 1, &match, 0)) {
        if (flags.many_files && !flags.suppress_file_names) {
          printf("%s:", filename);
        }
        if (flags.lines_number) {
          printf("%d:", line_number);
        }

        printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
               remaining + match.rm_so);

        remaining += match.rm_eo;
      }
    }
  } else {
    // List matched lines
    while ((read = getline(&line, &length, file)) > 0) {
      line_number++;

      if (regexec(regex, line, 1, &match, 0) == is_match) {
        if (flags.many_files && !flags.suppress_file_names) {
          printf("%s:", filename);
        }
        if (flags.lines_number) {
          printf("%d:", line_number);
        }

        printf("%s", line);

        // In case if a file end doesn't have the '\n'
        if (line[read - 1] != '\n') printf("\n");
      }
    }
  }

  free(line);
}

void add_pattern(Flags *flags, char *pattern) {
  if (flags->pattern == NULL) {
    flags->pattern = xmalloc(strlen(pattern) + 1);
    strcpy(flags->pattern, pattern);
  } else {
    flags->pattern =
        xrealloc(flags->pattern, strlen(flags->pattern) + strlen(pattern) + 2);
    strcat(flags->pattern, "|");
    strcat(flags->pattern, pattern);
  }
}

void add_pattern_from_file(Flags *flags, char *filename) {
  char *line = NULL;
  size_t length = 0;
  ssize_t read;

  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    if (!flags->suppress_errors) perror(filename);
  }

  while ((read = getline(&line, &length, file)) > 0) {
    if (line[read - 1] == '\n') line[read - 1] = '\0';

    add_pattern(flags, line);
  }

  fclose(file);
  free(line);
}

// utilities
void *xmalloc(size_t size) {
  void *ptr = malloc(size);

  if (ptr == NULL) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }

  return ptr;
}

void *xrealloc(void *ptr, size_t new_size) {
  void *new_ptr = realloc(ptr, new_size);

  if (new_ptr == NULL) {
    perror("Memory reallocation failed");
    free(ptr);
    exit(EXIT_FAILURE);
  }

  return new_ptr;
}