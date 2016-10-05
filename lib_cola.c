#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_fichero.h"

#define TRUE 1
#define FALSE 0

void queue(proc_queue proc, process* elem)
{
	int i=0;

	while(proc[i]!=NULL) {
		i++;
	}

	proc[i]=elem;
}

void move(proc_queue proc)
{
	int i=0,j=1;

	while(proc[i] != NULL) {
		proc[i] = proc[j];
		i++;
		j++;
	}
	proc[i-1] = NULL;
}

process* next(proc_queue proc)
{
	process* aux=NULL;
	aux = proc[0];

	return aux;
}

int is_null(proc_queue proc)
{
	if(proc[0] == NULL) {
		return TRUE;
	}
	return FALSE;
}
