#include "pid_list.h"
#include "valid_malloc.h"
#include <wait.h>

void PidList_init(PidList *list)
{
	list->size = 0;
	list->start = NULL;
}

void PidList_insert(PidList *list, pid_t pid)
{
	// initialize new node
	PidNode *node = VALID_MALLOC(sizeof(PidNode));
	node->pid = pid;
	node->prev = NULL;
	node->next = list->start;

	// set new node as the start of list
	list->size++;
	list->start = node;
}

void PidList_reap(PidList *list)
{
	PidNode *node = list->start;

	// iterate through all nodes in the list
	while(node != NULL)
	{

		// if process is successfully reaped
		if(waitpid(node->pid, NULL, WNOHANG))
		{
			// move the current node pointer
			PidNode *reaped_process = node;
			node = node->next;

			// remove and deallocate the node whose process was just reaped
			PidList_removeNode(list, reaped_process);
		}
		else
		{
			// move the current node pointer
			node = node->next;
		}
	}
}

void PidList_removeNode(PidList *list, PidNode *node)
{
	// remove node and relink list accordingly
	if(node->prev != NULL) node->prev->next = node->next;
	else list->start = node->next;
	if(node->next != NULL) node->next->prev = node->prev;

	// free node and decrement list size
	list->size--;
	free(node);
}

void PidList_deinit(PidList *list, int signal)
{
	PidNode *node = list->start;

	// iterate through all nodes in the list
	while(node != NULL)
	{

		// if process is successfully reaped
		if(waitpid(node->pid, NULL, WNOHANG))
		{
			// move the current node pointer
			PidNode *reaped_process = node;
			node = node->next;

			// remove and deallocate the node whose process was just reaped
			PidList_removeNode(list, reaped_process);
		}

		// if the process is still running
		else
		{
			// kill the process
			kill(node->pid, signal);

			// move the current node pointer
			PidNode *killed_process = node;
			node = node->next;

			// remove and deallocate the node whose process was just killed
			PidList_removeNode(list, killed_process);
		}
	}
}
