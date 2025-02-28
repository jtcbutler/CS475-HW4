#ifndef DSH_H_DEFINED
#define DSH_H_DEFINED

#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include "dynamic_string.h"
#include "pid_list.h"

#define INPUT_MAX 256
#define PATH_DELIMETER ':'
#define INPUT_DELIMETER ' '

extern DynamicString path_buffer;
extern PidList process_list;
extern char **path_segments;
extern char *input_buffer;

void dsh();
char** split(char *input, char delimeter);
int fgetsWrapper(char *buffer, int size, FILE *stream);
bool getExecutablePath(char *name, char **posibilities, DynamicString *path_buffer);
void pwd(DynamicString *path_buffer);
void cd(char *directory);
void runBinary(DynamicString *path_buffer, char **path_segments, char **input_segments);
void shellInit();
void shellDeinit();
void freeStringArray(char **string_array);

#endif
