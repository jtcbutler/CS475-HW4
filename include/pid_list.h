#ifndef PID_LIST_H_INCLUDED
#define PID_LIST_H_INCLUDED

#include <sys/types.h>

typedef struct
PidNode
{
	pid_t pid;
	struct PidNode *prev;	// I don't know why I made this doubly linked, there may be a reason but I don't remember it
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

/*
 * Initialize a PidList
 *
 * @arg <PidList*> list - the list to initialize
*/
void PidList_init(PidList *list);

/*
 * Reap all zombie processes in a PidList
 *
 * @arg <PidList*> list - the list whose zombie processes should be reaped
*/
void PidList_reap(PidList *list);

/*
 * Insert new process into a PidList
 *
 * @arg <PidList*> list - the list to insert the process into
 * @arg <pid_t> pid - the id of the process to be inserted
 */
void PidList_insert(PidList *list, pid_t pid);

/*
 * Reap/kill all processes with a PidList
 * Deallocate all memory used by the list
 *
 * @arg <PidList*> list - the list to deinitialize
 * @arg <int> signal - the signal that should be used to kill running processes
*/
void PidList_deinit(PidList *list, int signal);

/*
 * Remove a node from a PidList
 *
 * @arg <PidList*> list - the list from which the node should be removed
 * @arg <PidNode*> node - the node to remove
*/
void PidList_removeNode(PidList *list, PidNode *node);

#endif
