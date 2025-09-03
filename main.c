#include <bits/types/stack_t.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>

#define DEFAULT_SIZE 1024

char *read_line() {

  unsigned int position = 0;
  char *line = malloc(DEFAULT_SIZE);

  if (!line){
    fprintf(stderr, "[ERROR] Couldn't allocate memory for the current line");
    exit(1);
  }

  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s >> ", cwd);
  }
  else {
    printf("NULL >> ");
  }

  char character;
  while(1){
    character = fgetc(stdin);
    if (character == EOF || character == '\n'){
      line[position] = '\0';
      break;
    }

    line[position] = character;
    position++;

    if (position >= DEFAULT_SIZE) {
      char *new_line = realloc(line, DEFAULT_SIZE * 2);
      if (!new_line) {
          fprintf(stderr, "[ERROR] Couldn't reallocate memory for a bigger line\n");
          free(line);
          exit(1);
      }
      line = new_line;
    }
  }

  return line;
}

char **parse_tokens(char *line) {
  const char *DELIMITERS = " \r\n\t";

  char **tokens = malloc(DEFAULT_SIZE * sizeof(char*));
  unsigned int position = 0;
  
  char *token = strtok(line, DELIMITERS);
  while (token != NULL){
    tokens[position] = token;
    position++;

    if (position >= DEFAULT_SIZE){
      char **new_tokens = realloc(tokens, DEFAULT_SIZE * 2 * sizeof(char*));
      if (!new_tokens) {
        fprintf(stderr, "[ERROR] Couldn't reallocate memory for more tokens\n");
        free(line);
        exit(1);
      }
      tokens = new_tokens;
    }

    token = strtok(NULL, DELIMITERS);
  }

  tokens[position] = NULL;
  return tokens;
}

int execute(char** args) {
  pid_t pid = fork(), wpid;
  int status;

  if (pid == -1){
    fprintf(stderr, "[ERROR] Couldn't create another process, fork() faults");
    return 1;
  }
  else if (pid == 0){
    execvp(args[0], args);
    return 0;
  }
  else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    return 1;
  }
}

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

char* builtin_str[] = {
  "cd", "quit", "help"
};

int (*builtin_func[])(char **) = {
  &cd, &quit, &help
};

int main() {
  printf("Shell started...\n");

  char* line;
  char **args;

  while(1) {
    line = read_line();
    if (!line) {
      fprintf(stderr, "[ERROR] Couldn't read the current line");
    }
    
    args = parse_tokens(line);
    if (!args) {
      fprintf(stderr, "[ERROR] Coudln't parse the current tokens");
    }

    for (int i = 0; i < sizeof(builtin_func)/sizeof(char*); i++){
      if (strcmp(builtin_str[i], args[0]) == 0){
        (*builtin_func[i])(args);
      }
    }
    execute(args);

  }
  free(line);
  free(args);
  
  return 0;
}