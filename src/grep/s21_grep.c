#define _GNU_SOURCE
#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Flags {
  int ignore_case;          // -i, ignore uppercase vs. lowercase.
  int invert_match;         // -v, invert match.
  int count_only_matching;  // -c, output count of matching lines only
  int list_files;           // -l, output matching files only.
  int number;  // -n, precede each matching line with a line number.
  int suppress_file_names;  // -h,output matching lines without preceding them
                            // by file names
  int suppress_errors;      // -s, suppress error messages about nonexistent or
                            // unreadable files.
  int output_only_matches;  // -o, output count of matching lines only
  // char *pattern;           // -e Pattern to search for
} Flags;

// void read_flags(int argc, char *argv[], Flags *flags);
void grep(int argc, char *argv[], Flags flags);
void grep_file(FILE *file, Flags flags, regex_t *preg);

int main(int argc, char *argv[]) {
  Flags flags = {};

  // read_flags(argc, argv, &flags);
  grep(argc, argv, flags);

  return 0;
}

// void read_flags(int argc, char *argv[], Flags *flags) {
//     int opt;

//     flags->number_nonblank = 0;
//     flags->show_ends = 0;
//     flags->number = 0;
//     flags->squeeze_blank = 0;
//     flags->show_tabs = 0;
//     flags->show_nonprinting = 0;
//     flags->ignore_case = 0;
//     flags->invert_match = 0;
//     flags->count = 0;
//     flags->list_files = 0;
//     flags->suppress_errors = 0;
//     flags->output_only_matches = 0;
//     flags->pattern = NULL;

//     while ((opt = getopt(argc, argv, "beEnstTvicvlso", long_options, NULL))
//     != -1) {
//         switch (opt) {
//             case 'b':
//                 flags->number_nonblank = 1;
//                 break;
//             case 'e':
//                 flags->pattern = argv[optind++];
//                 break;
//             case 'i':
//                 flags->ignore_case = 1;
//                 break;
//             case 'v':
//                 flags->invert_match = 1;
//                 break;
//             case 'c':
//                 flags->count = 1;
//                 break;
//                         case 'l':
//                 flags->list_files = 1;
//                 break;
//             case 's':
//                 flags->suppress_errors = 1;
//                 break;
//             case 'o':
//                 flags->output_only_matches = 1;
//                 break;
//             case 'E':
//                 flags->show_ends = 1;
//                 break;
//             case 'n':
//                 flags->number = 1;
//                 break;
//             case 's':
//                 flags->squeeze_blank = 1;
//                 break;
//             case 't':
//                 flags->show_tabs = 1;
//                 break;
//             case 'T':
//                 flags->show_tabs = 1;
//                 break;
//             case 'v':
//                 flags->show_nonprinting = 1;
//                 break;
//             case '?':
//                 fprintf(stderr, "Invalid option: %s\n", argv[optind - 1]);
//                 exit(EXIT_FAILURE);
//                 break;
//         }
//     }
// }

void grep(int argc, char *argv[], Flags flags) {
  char **pattern = &argv[1];
  char **end = &argv[argc];
  regex_t preg_storage;
  regex_t *preg = &preg_storage;

  for (; pattern != end && pattern[0][0] == '-'; pattern++)
    ;
  if (pattern == end) {
    fprintf(stderr, "No pattern provided\n");
    exit(1);
  }
  if (regcomp(preg, *pattern, 0)) {
    fprintf(stderr, "Failed to compile regular excretion");
    exit(1);
  }

  for (char **filename = pattern + 1; filename != end; filename++) {
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
  (void)flags;

  while ((read = getline(&line, &length, file)) > 0) {
    if (!regexec(preg, line, 1, &match, 0)) {
      printf("%s", line);
    }
  }

  free(line);
}