#ifndef _QUEUE_H_
#define _QUEUE_H_

struct queue_entry_ {
	void* data;
	struct queue_entry_* next;
};

typedef struct queue_entry_ entry;

struct queue_ {
	int size;
	entry* first;
	entry* last;
};

typedef struct queue_ proc_queue;

proc_queue* queue_new(void);
void* queue_next(proc_queue* proc);
void queue_add(proc_queue* proc, void* elem);
void queue_move(proc_queue* proc);
void queue_remove(proc_queue* proc);
int is_queue_empty(proc_queue* proc);
int queue_size(proc_queue* proc);
void queue_free(proc_queue** queue,void (*free_data_cb)(void*));

#endif /*_QUEUE_H_*/