#ifndef PID_LIST_H_INCLUDED
#define PID_LIST_H_INCLUDED

#include <sys/types.h>

typedef struct
PidNode
{
	pid_t pid;
	struct PidNode *prev;
	struct PidNode *next;
}
PidNode;

typedef struct
PidList
{
	size_t size;
	struct PidNode *start;
}
PidList;

void PidList_init(PidList *list);
void PidList_reap(PidList *list);
void PidList_free(PidList *list, int signal);
void PidList_insert(PidList *list, pid_t pid);
void PidList_removeNode(PidList *list, PidNode *node);

#endif
