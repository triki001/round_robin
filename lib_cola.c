#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_fichero.h"

#define TRUE 1
#define FALSE 0

void encolar(proc_cola proc,process * elem)
{
	int i=0;

	while(proc[i]!=NULL)
		i++;
	proc[i]=elem;

}

void avanzar(proc_cola proc)
{
	int i=0,j=1;

	while(proc[i]!=NULL)
	{
		proc[i]=proc[j];
		i++;
		j++;
	}
	proc[i-1]=NULL;
}

process * sacar(proc_cola proc)
{
	process * aux=NULL;
	aux = proc[0];

	return aux;
}

int is_null(proc_cola proc)
{
	if(proc[0]==NULL)
		return TRUE;
	else
		return FALSE;
}
