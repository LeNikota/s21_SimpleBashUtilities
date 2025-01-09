#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Flags {
  int number_nonblank;   // -b, --number-nonblank, overrides -n
  int show_ends;         // -E, --show-ends
  int number;            // -n, --number
  int squeeze_blank;     // -s, --squeeze-blank
  int show_tabs;         // -T, --show-tabs
  int show_nonprinting;  // -v, --show-nonprinting
} Flags;

void read_flags(int argc, char *argv[], Flags *flags);
void set_table(const char *table[static 256]);
void set_table_nonprinting(const char *table[static 256]);
void cat(int argc, char *argv[], Flags flags, const char *table[static 256]);
void cat_file(FILE *file, Flags flags, const char *table[static 256]);

int main(int argc, char *argv[]) {
  Flags flags;
  const char *table[256];

  read_flags(argc, argv, &flags);
  set_table(table);

  if (flags.show_ends) table['\n'] = "$\n";
  if (flags.show_tabs) table['\t'] = "^I";
  if (flags.show_nonprinting) set_table_nonprinting(table);

  cat(argc, argv, flags, table);

  return 0;
}

void read_flags(int argc, char *argv[], Flags *flags) {
  int opt;
  struct option long_options[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                  {"number", no_argument, NULL, 'n'},
                                  {"squeeze-blank", no_argument, NULL, 's'},
                                  {0, 0, 0, 0}};

  flags->number_nonblank = 0;
  flags->show_ends = 0;
  flags->number = 0;
  flags->squeeze_blank = 0;
  flags->show_tabs = 0;
  flags->show_nonprinting = 0;

  while ((opt = getopt_long(argc, argv, "beEnstTv", long_options, NULL)) !=
         -1) {
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
        fprintf(stderr, "%s", argv[optind - 1]);
        exit(EXIT_FAILURE);  //? Ask someone, is it propare way to terminate the
                             // program on invalid input.
        break;
    }
  }
}

void set_table(const char *table[static 256]) {
  const char *origin_table[] = {
      "\00",  "\01",  "\02",  "\03",  "\04",  "\05",  "\06",  "\07",  "\b",
      "\t",   "\n",   "\v",   "\014", "\r",   "\016", "\017", "\020", "\021",
      "\022", "\023", "\024", "\025", "\026", "\027", "\030", "\031", "\032",
      "\033", "\034", "\035", "\036", "\037", " ",    "!",    "\"",   "#",
      "$",    "%",    "&",    "\'",   "(",    ")",    "*",    "+",    ",",
      "-",    ".",    "/",    "0",    "1",    "2",    "3",    "4",    "5",
      "6",    "7",    "8",    "9",    ":",    ";",    "<",    "=",    ">",
      "?",    "@",    "A",    "B",    "C",    "D",    "E",    "F",    "G",
      "H",    "I",    "J",    "K",    "L",    "M",    "N",    "O",    "P",
      "Q",    "R",    "S",    "T",    "U",    "V",    "W",    "X",    "Y",
      "Z",    "[",    "\\",   "]",    "^",    "_",    "`",    "a",    "b",
      "c",    "d",    "e",    "f",    "g",    "h",    "i",    "j",    "k",
      "l",    "m",    "n",    "o",    "p",    "q",    "r",    "s",    "t",
      "u",    "v",    "w",    "x",    "y",    "z",    "{",    "|",    "}",
      "~",    "\x7f", "\x80", "\x81", "\x82", "\x83", "\x84", "\x85", "\x86",
      "\x87", "\x88", "\x89", "\x8a", "\x8b", "\x8c", "\x8d", "\x8e", "\x8f",
      "\x90", "\x91", "\x92", "\x93", "\x94", "\x95", "\x96", "\x97", "\x98",
      "\x99", "\x9a", "\x9b", "\x9c", "\x9d", "\x9e", "\x9f", "\xa0", "\xa1",
      "\xa2", "\xa3", "\xa4", "\xa5", "\xa6", "\xa7", "\xa8", "\xa9", "\xaa",
      "\xab", "\xac", "\xad", "\xae", "\xaf", "\xb0", "\xb1", "\xb2", "\xb3",
      "\xb4", "\xb5", "\xb6", "\xb7", "\xb8", "\xb9", "\xba", "\xbb", "\xbc",
      "\xbd", "\xbe", "\xbf", "\xc0", "\xc1", "\xc2", "\xc3", "\xc4", "\xc5",
      "\xc6", "\xc7", "\xc8", "\xc9", "\xca", "\xcb", "\xcc", "\xcd", "\xce",
      "\xcf", "\xd0", "\xd1", "\xd2", "\xd3", "\xd4", "\xd5", "\xd6", "\xd7",
      "\xd8", "\xd9", "\xda", "\xdb", "\xdc", "\xdd", "\xde", "\xdf", "\xe0",
      "\xe1", "\xe2", "\xe3", "\xe4", "\xe5", "\xe6", "\xe7", "\xe8", "\xe9",
      "\xea", "\xeb", "\xec", "\xed", "\xee", "\xef", "\xf0", "\xf1", "\xf2",
      "\xf3", "\xf4", "\xf5", "\xf6", "\xf7", "\xf8", "\xf9", "\xfa", "\xfb",
      "\xfc", "\xfd", "\xfe", "\xff",
  };

  memcpy(table, origin_table, sizeof origin_table);
}

void set_table_nonprinting(const char *table[static 256]) {
  const char *sample1[] = {"^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "H"};
  const char *sample2[] = {"^K", "^L", "^M", "^N",  "^O", "^P", "^Q",
                           "^R", "^S", "^T", "^U",  "^V", "^W", "^X",
                           "^Y", "^Z", "^[", "^\\", "^]", "^^", "^_"};
  const char *sample3[] = {
      "^?",   "M-^@", "M-^A",  "M-^B", "M-^C", "M-^D", "M-^E", "M-^F", "M-^G",
      "M-^H", "M-^I", "M-^J",  "M-^K", "M-^L", "M-^M", "M-^N", "M-^O", "M-^P",
      "M-^Q", "M-^R", "M-^S",  "M-^T", "M-^U", "M-^V", "M-^W", "M-^X", "M-^Y",
      "M-^Z", "M-^[", "M-^\\", "M-^]", "M-^^", "M-^_", "M- ",  "M-!",  "M-\"",
      "M-#",  "M-$",  "M-%",   "M-&",  "M-'",  "M-(",  "M-)",  "M-*",  "M-+",
      "M-,",  "M--",  "M-.",   "M-/",  "M-0",  "M-1",  "M-2",  "M-3",  "M-4",
      "M-5",  "M-6",  "M-7",   "M-8",  "M-9",  "M-:",  "M-;",  "M-<",  "M-=",
      "M->",  "M-?",  "M-@",   "M-A",  "M-B",  "M-C",  "M-D",  "M-E",  "M-F",
      "M-G",  "M-H",  "M-I",   "M-J",  "M-K",  "M-L",  "M-M",  "M-N",  "M-O",
      "M-P",  "M-Q",  "M-R",   "M-S",  "M-T",  "M-U",  "M-V",  "M-W",  "M-X",
      "M-Y",  "M-Z",  "M-[",   "M-\\", "M-]",  "M-^",  "M-_",  "M-`",  "M-a",
      "M-b",  "M-c",  "M-d",   "M-e",  "M-f",  "M-g",  "M-h",  "M-i",  "M-j",
      "M-k",  "M-l",  "M-m",   "M-n",  "M-o",  "M-p",  "M-q",  "M-r",  "M-s",
      "M-t",  "M-u",  "M-v",   "M-w",  "M-x",  "M-y",  "M-z",  "M-{",  "M-|",
      "M-}",  "M-~",  "M-^?"};

  memcpy(table, sample1, sizeof(sample1));
  memcpy(&table[11], sample2, sizeof(sample2));
  memcpy(&table['~' + 1], sample3, sizeof(sample3));
}

void cat(int argc, char *argv[], Flags flags, const char *table[static 256]) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') continue;

    FILE *file = fopen(argv[i], "rb");

    if (file == NULL) {
      fprintf(stderr, "%s", argv[0]);
      perror(argv[i]);
      continue;
    }

    cat_file(file, flags, table);

    fclose(file);
  }
}

void cat_file(FILE *file, Flags flags, const char *table[static 256]) {
  int c = 0;
  int last = '\n';
  static int newline_count = 0;
  static int line_number = 0;

  while (fread(&c, sizeof(char), 1, file) > 0) {
    if (last == '\n') {
      newline_count++;

      // Skip blank lines
      if (flags.squeeze_blank && newline_count > 1 && c == '\n') continue;

      // Number lines
      if (flags.number_nonblank) {
        // Nonblank
        if (c != '\n') {
          printf("%6i\t", ++line_number);
        }
        // All
      } else if (flags.number) {
        printf("%6i\t", ++line_number);
      }

    } else {
      newline_count = 0;
    }

    if (!*table[c])
      printf("%c", '\0');
    else
      printf("%s", table[c]);
    last = c;
  }
}