#include "valid_malloc.h"
#include <stdio.h>

void* validMalloc(size_t size, size_t line, const char *file_name, const char *func_name)
{
	// attempt allocate requested memory
	void *ptr = malloc(size);

	// if the allocation failed, print a verbose error message and exit
	if(!ptr)
	{
		fprintf(stderr, "MEMORY ALLOCATION FAILURE:\n");
		fprintf(stderr, "\tmalloc() failed to allocate a block of size %lu bytes\n", size);
		fprintf(stderr, "\tfailure occured on line %lu of file \"%s\" inside of function %s()\n", line, file_name, func_name);
		fprintf(stderr, "\texiting...\n");
		exit(EXIT_FAILURE);
	}

	// return the allocated memory
	return ptr;
}
