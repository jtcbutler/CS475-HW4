#include <sys/resource.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include "dsh.h"

#define MAX_PROC 1024

int main(void) 
{
	// DO NOT REMOVE THE BLOCK BELOW (FORK BOMB PREVENTION) //
	struct rlimit limit;
	limit.rlim_cur = MAX_PROC;
	limit.rlim_max = MAX_PROC;
	setrlimit(RLIMIT_NPROC, &limit);
	// DO NOT REMOVE THE BLOCK ABOVE THIS LINE //
	
	dsh();

	return EXIT_SUCCESS;
}
