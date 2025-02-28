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
	PidNode *node = VALID_MALLOC(sizeof(PidNode));
	node->pid = pid;
	node->prev = NULL;
	node->next = list->start;

	list->size++;
	list->start = node;
}

void PidList_reap(PidList *list)
{
	PidNode *node = list->start;
	while(node != NULL)
	{
		if(waitpid(node->pid, NULL, WNOHANG))
		{
			PidNode *reaped_process = node;
			node = node->next;

			PidList_removeNode(list, reaped_process);
		}
		else
		{
			node = node->next;
		}
	}
}

void PidList_removeNode(PidList *list, PidNode *node)
{
	if(node->prev != NULL) node->prev->next = node->next;
	else list->start = node->next;

	if(node->next != NULL) node->next->prev = node->prev;

	list->size--;

	free(node);
}

void PidList_free(PidList *list, int signal)
{
	PidNode *node = list->start;
	while(node != NULL)
	{
		if(waitpid(node->pid, NULL, WNOHANG))
		{
			PidNode *reaped_process = node;
			node = node->next;

			PidList_removeNode(list, reaped_process);
		}
		else
		{
			kill(node->pid, signal);

			PidNode *killed_process = node;
			node = node->next;

			PidList_removeNode(list, killed_process);
		}
	}
}
