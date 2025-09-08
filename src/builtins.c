#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "builtins.h"
#include "./history/history.h"

int cd(char **args){
  if (args[1] == NULL){
    perror("[ERROR] Expected a path for the cd comamnd");
    return 1;
  }

  if (chdir(args[1]) != 0){
    perror("[ERROR] Couldn't change directory");
    return 1;
  }
  
  return 0;
}

int quit(char **args){
  exit(1);
  return 0;
}

int help(char **args){
  printf("Shell made by Gianluca Russo...\n");
  printf("In about 2 hours...\n\n");
  return 0;
}

int history(char **args){
  FILE *file = fopen(HISTORY_FILE_PATH, "r");
  if (!file) {
    perror("[ERROR] Couldn't read history file");
    return 1;
  }

  fseek(file, 0, SEEK_SET);

  char character;
  while ((character = fgetc(file)) != EOF){
    printf("%c", character);
  }

  return 0;
}

char* builtin_str[] = {
  "cd", "quit", "help",
  "history",
};

int (*builtin_func[])(char **) = {
  &cd, &quit, &help,
  &history,

  NULL
};