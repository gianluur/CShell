//utils.c

// C Standard Library
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <limits.h>
#include "string.h"

// Include Header
#include "utils.h"

struct termios normal_terminal;

void enable_raw_mode() {
  tcgetattr(STDIN_FILENO, &normal_terminal);
  struct termios raw_terminal = normal_terminal;
  raw_terminal.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &raw_terminal);
}

void disable_raw_mode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &normal_terminal);
}

void print_current_directory() {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s >> ", cwd);
  }
  else {
    printf("NULL >> ");
  }
}

void reverse_line(char *line) {
  for(int i = 0, j = strlen(line) - 1; i < j; i++, j--){
    char current_character = line[i];
    line[i] = line[j];
    line[j] = current_character;
  }
}

long get_file_size(FILE *file) {
  fseek(file, 0, SEEK_END);
  return ftell(file);
}