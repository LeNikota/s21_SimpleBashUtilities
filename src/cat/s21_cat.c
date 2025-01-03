#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct Flags {
  int number_nonblank;   // -b, --number-nonblank, overrides -n
  int show_ends;         // -E, --show-ends
  int number;            // -n, --number
  int squeeze_blank;     // -s, --squeeze-blank
  int show_tabs;         // -T, --show-tabs
  int show_nonprinting;  // -v, --show-nonprinting
} Flags;

void read_flags(int argc, char *argv[], Flags *flags);

int main(int argc, char *argv[]) {
  Flags flags;

  read_flags(argc, argv, &flags);

  return 0;
}

void read_flags(int argc, char *argv[], Flags *flags) {
  int opt;
  struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {0, 0, 0, 0}
  };

  flags->number_nonblank = 0;
  flags->show_ends = 0;
  flags->number = 0;
  flags->squeeze_blank = 0;
  flags->show_tabs = 0;
  flags->show_nonprinting = 0;

  while ((opt = getopt_long(argc, argv, "beEnstTv", long_options, NULL)) != -1) {
    switch (opt) {
      case 'b':
        flags->number_nonblank = 1;
        break;
        case 'e':
        flags->show_ends = 1;
        flags->show_nonprinting = 1;
        break;
      case 'E':
        flags->show_ends = 1;
        break;
      case 'n':
        flags->number = 1;
        break;
      case 's':
        flags->squeeze_blank = 1;
        break;
        case 't':
        flags->show_tabs = 1;
        flags->show_nonprinting = 1;
        break;
      case 'T':
        flags->show_tabs = 1;
        break;
      case 'v':
        flags->show_nonprinting = 1;
        break;
      case '?':
        fprintf(stderr, "Unknown option: %s\n", argv[optind - 1]);
        exit(EXIT_FAILURE); //? Ask someone, is it propare way to terminate the program on invalid input.
        break;
    }
  }
}