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

/*
 * Run a new instance of DavidShell™
*/
void dsh();

/*
 * Pring the current directory
*/
void pwd();

/*
 * Initialize global variables needed for dsh
*/
void shellInit();

/*
 * Deinitialize global variables used for dsh
*/
void shellDeinit();

/*
 * Change the current directory
 *
 * @arg <char*> directory - the directory to change to
*/
void cd(char *directory);

/*
 * Get the path to a given executable
 *
 * @arg <char*> name - the name of the executable to find
 */
bool getExecutablePath(char *name);

/*
 * Free a string array
 *
 * @arg <char**> string_array - the array to free
*/
void freeStringArray(char **string_array);

/*
 * Wrapper for fgets() with simple error checks
 *
 * @arg <char*> buffer - the buffer that should be read into
 * @arg <int> size - the size of buffer
 * @arg <FILE*> stream - the file stream to read from
*/
int fgetsWrapper(char *buffer, int size, FILE *stream);

/*
 * Split a string on a given delimeter character
 *
 * @arg <char*> input - the string to split
 * @arg <char> delimeter - the delimeter character
 * @ret <size_t*> output_length - the length of the allocated array (does not include NULL terminator)
 * @ret <char**> - the split string array
*/
char** split(char *input, char delimeter, size_t *output_length);

/*
 * Run a binary
 *
 * @arg <char**> input_segments - the binaries name and arguments
 * @arg <size_t> input_segments_length - the length of input_segments
*/
void runBinary(char **input_segments, size_t input_segments_length);

#endif
