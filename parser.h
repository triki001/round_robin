#ifndef _PARSER_H_
#define _PARSER_H_

#include "queue.h"

struct process_ {
	int time;
	int pid;
	char** arg;
};

typedef struct process_ process;

proc_queue* load_from_file(const char* filename);
void free_process_data(void* proc);

#endif /*_PARSER_H_*/
