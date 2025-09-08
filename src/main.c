//main.c

// C Standard Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

// Linux headers
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>

// Project Headers
#include "./builtins.h"
#include "./utils.h"
#include "./history/history.h"

#define DEFAULT_SIZE 1024

char *read_line(FILE *history) {
  unsigned int position = 0;
  char *line = malloc(DEFAULT_SIZE);

  if (!line){
    fprintf(stderr, "[ERROR] Couldn't allocate memory for the current line");
    exit(1);
  }

  print_current_directory();

  char input_character;
  while(1){
    input_character = getchar();

    // Handles the termination of the line
    if (input_character == EOF || input_character == '\n'){
      line[position] = '\0';
      break;
    }

    // Handles up and down arrow for history
    // if (character == '\x1b'){
    //   character = getchar();  
    //   if (character == '[') { 
    //     static int i = 0;

    //     char *line = malloc(DEFAULT_SIZE);
    //     long position = 0;
    //     static long last_line_size = 0; 
    //     long file_length;

    //     character = getchar();
    //     if (character == 'A') {
    //       fseek(history, 0, SEEK_END);
    //       file_length = ftell(history);
          
    //       i++;
    //       char character;
    //       for(; i <= file_length; i++){
    //         fseek(history, -i, SEEK_END);
    //         character = fgetc(history);
    //         line[position++] = character;
            
    //         if (character == '\n' || i == file_length) {
    //           line[position] = '\0';
              
    //           reverse_line(line);
              
    //           if (i == file_length){
    //             printf("\n");
    //           }
    //           printf("%s", line);
              
    //           position = 0;
    //           last_line_size = strlen(line);
    //           break;
    //         }
    //       }
    //       free(line);
    //     }
    //     else if (character == 'B') {

    //       static bool fexec = true;
    //       if (fexec){
    //         fseek(history, last_line_size, SEEK_CUR);
    //         fexec = false;
    //       }


    //       line = fgets(line, DEFAULT_SIZE, history);
    //       if (line) {
    //         printf("%s", line);
    //       }
    //     }
    //   }
    //   continue;
    // }

    if (input_character == '\x1b' && (input_character = getchar()) == '[') {
      char *history_line = malloc(DEFAULT_SIZE);
      if (!line) {
        perror("[ERROR] Couldn't allocate memory for history line");
        exit(1);
      }
      long history_position = 0;
      
      const long history_size = get_file_size(history);
      if (history_size == -1) {
        perror("[ERROR] Couldn't obtain history file size");
        exit(1);
      }

      static int history_index = 0;

      char file_character;
      while(1) {
        input_character = getchar();
        if (input_character == 'A') {
          printf("arrow up");
        }
        
        else if (input_character == 'B') {
          printf("arrow down");
        }

        break;
      }

      free(history_line);
      continue;
    }

    // Handles backspace
    if (input_character == '\b' || input_character == 127){
      if (position > 0) {
        position--;
        line[position] = '\0';
        printf("\b \b");
        fflush(stdout);
      }
      continue;
    }

    line[position] = input_character;
    position++;

    printf("%c", input_character);

    // Handles the resizing of the line buffer in case it overflows
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

  printf("\n");
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
  pid_t pid = fork();
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
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    return 1;
  }
}

FILE *start_history() {
  FILE *file = fopen(HISTORY_FILE_PATH, "a+");
  if (!file) {
    perror("[ERROR]Couldn't open/create history file");
    exit(1);
  }
  
  return file;
}

int main() {
  printf("Starting Shell...\n");

  enable_raw_mode();
  FILE *history = start_history();

  while(1) {
    char *line = read_line(history);
    if (!line) {
      fprintf(stderr, "[ERROR] Couldn't read the current line");
      continue;
    }

    fputs(line, history);
    fputc('\n', history);
    fflush(history);
    
    char **args = parse_tokens(line);
    if (!args) {
      fprintf(stderr, "[ERROR] Coudln't parse the current tokens");
      continue;
    }

    bool executed_bultin = false;
    for (int i = 0; builtin_func[i] != NULL; i++){
      if (strcmp(builtin_str[i], args[0]) == 0){
        (*builtin_func[i])(args);
        executed_bultin = true;
        break;
      }
    }

    if (!executed_bultin){
      execute(args);
    }

    free(line);
    free(args);
  }

  fclose(history);
  disable_raw_mode();
  
  return 0;
}