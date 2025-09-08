#ifndef BUILTINS_C
#define BUILTINS_C

int cd(char **args);
int quit(char **args);
int help(char **args);

extern char* builtin_str[];
extern int (*builtin_func[])(char **);

#endif