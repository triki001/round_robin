#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define TRUE 1
#define FALSE 0

static void queue_delete_all_elements(proc_queue* queue, void (*free_data_cb)(void*));

proc_queue* queue_new(void)
{
	proc_queue* queue = (proc_queue*)malloc(sizeof(proc_queue));
	if (queue != NULL) {
		queue->size = 0;
		queue->first = NULL;
		queue->last = NULL;
	}
	return queue;
}

void queue_add(proc_queue* queue, void* elem)
{
	entry* new_entry = NULL;

	if (queue == NULL  || elem == NULL) {
		return;
	}

	new_entry = (entry*) malloc(sizeof(entry));
	new_entry->data = elem;
	new_entry->next = NULL;

	if (queue->first == NULL) {
		queue->first = new_entry;
		new_entry->next = NULL;
	} else {
		queue->last->next = new_entry;
	}
	queue->last = new_entry;
	queue->size += 1;
}

void queue_move(proc_queue* queue)
{
	entry* aux = NULL;

	if (queue == NULL || queue->size == 1) {
		return;
	}

	aux = queue->first->next;
	queue->last->next = queue->first;
	queue->last = queue->last->next;
	queue->first->next = NULL;
	queue->first = aux;
}

void* queue_next(proc_queue* queue)
{
	if (queue == NULL) {
		return NULL;
	}

	return queue->first->data;
}

int is_queue_empty(proc_queue* proc)
{
	if (proc->size == 0) {
		return TRUE;
	}
	return FALSE;
}

int queue_size(proc_queue* proc)
{
	if (proc == NULL) {
		return 0;
	}
	return proc->size;
}

void queue_remove(proc_queue* proc)
{
	entry* aux = NULL;

	if (proc == NULL || is_queue_empty(proc)) {
		return;
	}
	
	if (proc->size == 1) {
		free(proc->first);
		proc->first = NULL;
		proc->last = NULL;
	} else {
		aux = proc->first;
		proc->first = proc->first->next;
		free(aux);
	}

	proc->size -= 1;
}

static void queue_delete_all_elements(proc_queue* queue, void (*free_data_cb)(void*))
{
	entry* current = NULL;
	entry* next = NULL;

	current = queue->first;
	while (current != NULL) {
		next = current->next;
		if (free_data_cb != NULL) {
			free_data_cb(current->data);
		}
		free(current);
		current = next;
	}

	queue->first = NULL;
	queue->last = NULL;
}

void queue_free(proc_queue** queue, void (*free_data_cb)(void*))
{
	if (queue == NULL || *queue == NULL) {
		return;
	}

	if ((*queue)->size > 0) {
		queue_delete_all_elements(*queue, free_data_cb);
	}

	free(*queue);
	*queue = NULL;
}