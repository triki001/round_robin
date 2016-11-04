#ifndef _LIB_FICHERO_H_
#define _LIB_FICHERO_H_

#include "lib_cola.h"

struct process_ {
	int time;
	int pid;
	char** arg;
};

typedef struct process_ process;

proc_queue* load_from_file(const char* filename);
char* save_to_file();
void free_process_data(void* proc);

#endif /*_LIB_FICHERO_H_*/
