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
{ shellInit();

	bool proceed = true;
	while(proceed){
		PidList_reap(&process_list);
		printf("dsh> ");

		int input_result = fgetsWrapper(input_buffer, INPUT_MAX, stdin);
		if(input_result == -1) 
		{
			fprintf(stderr, "dsh: failed to read input\n");
			break;
		}
		else if(input_result == -2) 
		{
			fprintf(stderr, "dsh: input too long\n");
			continue;
		}
		else
		{
			char **input_segments = split(input_buffer, INPUT_DELIMETER);

			if(input_segments[0] == NULL) continue;
			else if(strcmp("exit", input_segments[0]) == 0) proceed = false;
			else if(strcmp("pwd", input_segments[0]) == 0) pwd(&path_buffer);
			else if(strcmp("cd", input_segments[0]) == 0) cd(input_segments[1]);
			else runBinary(&path_buffer, path_segments, input_segments);

			freeStringArray(input_segments);
		}
	}

	shellDeinit();
}

void shellInit()
{
	DynamicString_initCapacity(&path_buffer, PATH_MAX);

	PidList_init(&process_list);

	input_buffer = VALID_MALLOC(sizeof(char) * INPUT_MAX);

	path_segments = split(getenv("PATH"), PATH_DELIMETER);

	int index = 0;
	while(path_segments[index] != NULL) index++;

	char **new_path_segments = VALID_MALLOC(sizeof(char*) * (index+2));
	char *current_dir = VALID_MALLOC(sizeof(char) * 2);
	strcpy(current_dir, ".");
	new_path_segments[0] = current_dir;

	index = 0;
	while(path_segments[index] != NULL)
	{
		new_path_segments[index+1] = path_segments[index];
		index++;
	}
	path_segments[index] = NULL;
	free(path_segments);
	path_segments = new_path_segments;
}

void shellDeinit()
{
	DynamicString_free(&path_buffer);

	PidList_free(&process_list, SIGHUP);

	freeStringArray(path_segments);

	free(input_buffer);
}

void freeStringArray(char **string_array)
{
	int index = 0;
	while(string_array[index] != NULL)
	{
		free(string_array[index]);
		index++;
	}
	free(string_array);
}

int fgetsWrapper(char *buffer, int size, FILE *stream)
{
	if(fgets(buffer, size, stream) == NULL) return -1;

	int len = strlen(buffer);

	if(buffer[len-1] != '\n')
	{
		while(getchar() != '\n');
		return -2;
	}

	buffer[len-1] = '\0';
	return len - 1;
}

char** split(char *input, char delimeter)
{
	int input_length = strlen(input);
	int number_of_words = 0;
	bool prior_space = true;

	for(int i = 0; i < input_length; i++)
	{
		if(input[i] != delimeter)
		{
			if(prior_space)
			{
				number_of_words++;
				prior_space = false;
			}	
		}
		else prior_space = true;
	}
	char** split_input = VALID_MALLOC(sizeof(char*) * (number_of_words + 1)); split_input[number_of_words] = NULL;

	int index = 0;
	for(int i = 0; i < number_of_words; i++)
	{
		while(input[index] == delimeter) index++;

		int length = 0;
		while(index+length < strlen(input) && input[index + length] != delimeter) length++;

		split_input[i] = VALID_MALLOC(sizeof(char) * (length + 1));

		for(int j = 0; j < length; j++)
		{
			split_input[i][j] = input[index+j];
		}
		split_input[i][length] = '\0';

		index+=length;
	}

	return split_input;
}

bool getExecutablePath(char *name, char **posibilities, DynamicString *path_buffer)
{
	int index = 0;
	while(posibilities[index] != NULL)
	{
		DIR *directory = opendir(posibilities[index]);

		if(directory == NULL)
		{
			index++;
			continue;
		}

		while(1)
		{
			struct dirent *entry = readdir(directory);
			if(entry == NULL) break;


			if(strcmp(entry->d_name, name) == 0)
			{
				DynamicString_clear(path_buffer);
				DynamicString_append(path_buffer, posibilities[index]);
				DynamicString_append(path_buffer, "/");
				DynamicString_append(path_buffer, entry->d_name);
				closedir(directory);
				return true;
			}
		}
		closedir(directory);
		index++;
	}
	return false;
}

void pwd(DynamicString *path_buffer)
{
	char* cwd = getcwd(path_buffer->content, path_buffer->capacity);
	printf("%s\n", cwd);
}

void cd(char *path)
{
	char *directory = (path == NULL) ? getenv("HOME") : path;
	if(chdir(directory))
	{
		fprintf(stderr, "dsh: cd: Unable to move to '%s'\n", directory);
	}
}

void runBinary(DynamicString *path_buffer, char **path_segments, char **input_segments)
{
	char* executable_path;

	if(input_segments[0][0] == '/' || input_segments[0][0] == '.')
	{
		executable_path = input_segments[0];
	}
	else
	{
		if(getExecutablePath(input_segments[0], path_segments, path_buffer))
		{
			executable_path = path_buffer->content;
		}
		else
		{
			fprintf(stderr, "dsh: unable to locate executable '%s'\n", input_segments[0]);
			return;
		}
	}

	int index = 0;
	while(input_segments[index] != NULL) index++;

	bool block = true;
	if(strcmp(input_segments[index-1], "&") == 0)
	{
		free(input_segments[index-1]);
		input_segments[index-1] = NULL;
		block = false;
	}

	pid_t process_id = fork();

	if(process_id == 0)
	{
		execv(executable_path, input_segments);
	}
	else if (process_id == -1)
	{
		fprintf(stderr, "dsh: failed to create child process\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		if(block) waitpid(process_id, NULL, 0);
		else PidList_insert(&process_list, process_id);
	}
}
