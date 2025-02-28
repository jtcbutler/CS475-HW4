#ifndef VALID_MALLOC_H_DEFINED
#define VALID_MALLOC_H_DEFINED

#include <stdlib.h>

// convenient wrapper for validMalloc()
#define VALID_MALLOC(size) validMalloc(size, __LINE__, __FILE__, __func__);

/*
 * Wrapper for malloc() that will print a verbose error message and exit the program upon allocation failure
 *
 * @arg size_t <size> - the number of bytes to allocate
 * @arg size_t <line> - the line number from which validMalloc() was called
 * @arg const char* <file_name> - the name of the file from which validMalloc() was called
 * @arg const char* <func_name> - the name of the function from which validMalloc() was called
 *
 * @ret void* - a pointer to the allocated memory
*/
void* validMalloc(size_t size, size_t line, const char *file_name, const char *func_name);

#endif
