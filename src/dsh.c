#include "dsh.h"
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <wait.h>
#include "dynamic_string.h"
#include "valid_malloc.h"

DynamicString path_buffer;
PidList process_list;
char **path_segments;
char *input_buffer;

void dsh()
{ 
	// initialize global variable
	shellInit();

	bool proceed = true;
	while(proceed){

		// scan the list of processes running in background
		// reap the processes that have completed execution
		PidList_reap(&process_list);

		printf("dsh> ");
		int input_result = fgetsWrapper(input_buffer, INPUT_MAX, stdin);

		// if scanf() failed
		if(input_result == -1) 
		{
			fprintf(stderr, "dsh: failed to read input\n");
			break;
		}

		// if the input length was greater than the buffer size
		else if(input_result == -2) 
		{
			fprintf(stderr, "dsh: input too long\n");
			continue;
		}

		// normal command execution
		else
		{
			// split the input on spaces
			size_t input_segments_length;
			char **input_segments = split(input_buffer, INPUT_DELIMETER, &input_segments_length);

			// if the input was entirely comprised of spaces
			if(input_segments[0] == NULL) continue;

			// if the input was a buildin command
			else if(strcmp("exit", input_segments[0]) == 0) proceed = false;
			else if(strcmp("pwd", input_segments[0]) == 0) pwd();
			else if(strcmp("cd", input_segments[0]) == 0) cd(input_segments[1]);

			// normal command execution
			else runBinary(input_segments, input_segments_length);

			freeStringArray(input_segments);
		}
	}

	// deinitialize global variables
	shellDeinit();
}

void shellInit()
{
	//initialize both path_buffer and process_list
	DynamicString_initCapacity(&path_buffer, PATH_MAX);
	PidList_init(&process_list);

	// allocate a buffer for user input
	input_buffer = VALID_MALLOC(sizeof(char) * INPUT_MAX);

	// split the path variable into a string array on the ':' character
	size_t path_segments_length;
	char **temp_path_segments = split(getenv("PATH"), PATH_DELIMETER, &path_segments_length);

	// allocate a new string array with room for one more element
	// allocate room for the string "." on the heap
	// place "." as the first element of path_segments
	path_segments = VALID_MALLOC(sizeof(char*) * (path_segments_length + 2));
	path_segments[path_segments_length+1] = NULL;
	char *dot = VALID_MALLOC(sizeof(char) * 2);
	path_segments[0] = dot;
	strcpy(dot, ".");

	// copy over the split elements of $PATH
	for(int i = 0; i < path_segments_length; i++)
	{
		path_segments[i+1] = temp_path_segments[i];
	}

	// free the old path_segments
	free(temp_path_segments);
}

void shellDeinit()
{
	// deallocate all global variables
	DynamicString_deinit(&path_buffer);
	PidList_deinit(&process_list, SIGHUP);
	freeStringArray(path_segments);
	free(input_buffer);
}

void freeStringArray(char **string_array)
{
	int index = 0;

	// free all string contents of the array
	while(string_array[index] != NULL)
	{
		free(string_array[index]);
		index++;
	}

	// free the array
	free(string_array);
}

int fgetsWrapper(char *buffer, int size, FILE *stream)
{
	// read input into buffer
	if(fgets(buffer, size, stream) == NULL) return -1;

	int len = strlen(buffer);

	// if the read input does not end with '\n'
	// the input was to long
	// clear input
	if(buffer[len-1] != '\n')
	{
		while(getchar() != '\n');
		return -2;
	}

	// truncate the newline
	buffer[len-1] = '\0';
	return len - 1;
}

char** split(char *input,  char delimeter, size_t *output_length)
{
	*output_length = 0;
	bool prior_space = true;

	// iterate through the string determining how man chunks there are
	// (as seperated by delimeter)
	for(int i = 0; i < strlen(input); i++)
	{
		if(input[i] != delimeter)
		{
			if(prior_space)
			{
				(*output_length)++;
				prior_space = false;
			}	
		}
		else prior_space = true;
	}

	// allocate an array large enough to hold all chunks and a NULL terminator
	char** split_input = VALID_MALLOC(sizeof(char*) * (*output_length + 1)); 
	split_input[*output_length] = NULL;

	// populate the previously allocated array with the chunks
	int index = 0;
	for(int i = 0; i < *output_length; i++)
	{
		// consume delimeter characters
		while(input[index] == delimeter) index++;

		// determine the length of the chunk
		int length = 0;
		while(index+length < strlen(input) && input[index + length] != delimeter) length++;

		// allocate space for the chunk
		split_input[i] = VALID_MALLOC(sizeof(char) * (length + 1));

		// copy over the chunk
		for(int j = 0; j < length; j++)
		{
			split_input[i][j] = input[index+j];
		}
		split_input[i][length] = '\0';

		// move index past the current chunk
		index+=length;
	}

	return split_input;
}

bool getExecutablePath(char *name)
{
	int index = 0;

	// iterate through all posibilities checking their contents against name
	while(path_segments[index] != NULL)
	{
		DIR *directory = opendir(path_segments[index]);

		// if the directory could not be opened
		if(directory == NULL)
		{
			index++;
			continue;
		}

		// check all contents of the current directory
		while(1)
		{
			struct dirent *entry = readdir(directory);
			if(entry == NULL) break;


			// if the current entrys name is the same as name
			// build an absolute path inside of path_buffer
			if(strcmp(entry->d_name, name) == 0)
			{
				DynamicString_clear(&path_buffer);
				DynamicString_append(&path_buffer, path_segments[index]);
				DynamicString_append(&path_buffer, "/");
				DynamicString_append(&path_buffer, entry->d_name);
				closedir(directory);
				return true;
			}
		}
		closedir(directory);
		index++;
	}
	return false;
}

void pwd()
{
	char* cwd = getcwd(path_buffer.content, path_buffer.capacity);
	printf("%s\n", cwd);
}

void cd(char *path)
{
	char *directory = (path == NULL) ? getenv("HOME") : path;
	if(chdir(directory))
	{
		fprintf(stderr, "dsh: cd: unable to move to '%s'\n", directory);
	}
}

void runBinary(char **input_segments, size_t input_segments_length)
{
	char* executable_path;

	// determine the executables path
	if(input_segments[0][0] == '/' || input_segments[0][0] == '.')
	{
		executable_path = input_segments[0];
	}
	else
	{
		if(getExecutablePath(input_segments[0]))
		{
			executable_path = path_buffer.content;
		}
		else
		{
			fprintf(stderr, "dsh: unable to locate executable '%s'\n", input_segments[0]);
			return;
		}
	}

	// if the final argument is '&'
	// free the final argument and set block to false
	bool block = true;
	if(strcmp(input_segments[input_segments_length-1], "&") == 0)
	{
		free(input_segments[input_segments_length-1]);
		input_segments[input_segments_length-1] = NULL;
		block = false;
	}

	// create child process
	pid_t process_id = fork();

	// if fork() failed
	if (process_id == -1)
	{
		fprintf(stderr, "dsh: failed to create child process\n");
		fprintf(stderr, "     this happens sometimes, I have no clue why\n");
		fprintf(stderr, "     I just restart my laptop ¯\\_(ツ)_/¯\n");
		exit(EXIT_FAILURE);
	}

	// in child
	else if(process_id == 0)
	{
		execv(executable_path, input_segments);
	}

	// in parent
	else
	{
		if(block) waitpid(process_id, NULL, 0);
		else PidList_insert(&process_list, process_id);
	}
}
