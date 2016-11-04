#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "parser.h"

#define TRUE 1
#define FALSE 0

static char* copy_line(const char* raw_file,int size);
static void save_into_struct(process* procStr,char** str);
static char* extract_from_file(const char* filename,int* lin);

proc_queue* load_from_file(const char* filename)
{
	char* raw_file = NULL;
	char* line = NULL;
	char** splited_line = NULL;
	process* proc_data = NULL;
	proc_queue* proc = NULL;
	int i = 0,j;
	int lines;
	
	raw_file = extract_from_file(filename, &lines);

	if (raw_file == NULL) {
		return NULL;
	}

	proc = queue_new();

	if(proc == NULL) {
		return NULL;
	}

	j = 0;
	while (raw_file[i] != '\0') {		
		if (raw_file[i] == '\n') {
			proc_data = malloc(sizeof(process));

			if (proc_data == NULL) {
				return NULL;
			}

			line = copy_line(raw_file, i);
			splited_line = str_tokenizer(line);
			free(line);

			proc_data->arg = (char**)malloc(sizeof(char*));

			save_into_struct(proc_data, splited_line);

			free(line);
			queue_add(proc, proc_data);
			raw_file += i + 1;
			i = 0;
			j++;
		} else {
			i++;
		}
	}

	return proc;
}

static char* copy_line(const char* raw_file,int size)
{
	char* line = NULL;

	if (raw_file == NULL) {
		return NULL;
	}

	line = malloc(sizeof(char)*(size+1));
	
	if (line == NULL) {
		return NULL;
	}

	strncpy(line, raw_file, size);
	line[size] = '\0';

	return line;
}

void free_process_data(void* proc)
{
	int i = 0;
	process* aux = (process*)proc;

	while(aux->arg[i] != NULL) {
		free(aux->arg[i]);
		i++;
	}

	free(aux->arg);
	free(aux);
	aux = NULL;
}

void save_into_struct(process* procStr,char** str)
{
	int t,j,i = 0;

	sscanf(str[0], "%d", &t);
	procStr->time = t;
	str++;

	while (str[i] != NULL) {
		j = i + 1;
		procStr->arg = realloc(procStr->arg, sizeof(char *)*j);
		procStr->arg[i] = malloc(sizeof(char)*strlen(str[i]));
		strcpy(procStr->arg[i], str[i]);
		i++;
	}
	j = i + 1;
	procStr->arg = realloc(procStr->arg, sizeof(char *)*j);
	procStr->arg[i] = NULL;
}

char* extract_from_file(const char* filename,int* lin)
{
	FILE* f = NULL;
	int lSize, i = 0, c = 0;
	char* aux = NULL;
	
	f = fopen(filename, "r");

	if (f == NULL)
	{
		printf("Error. File not found.\n\n");
		exit(-1);
	}

	fseek(f, 0, SEEK_END);
	lSize = ftell(f);
	rewind(f);
	
	aux = malloc(sizeof(char)*(lSize+1));

	if (aux == NULL) {
		printf("Error. No memory available\n");
		return NULL;
	}

	fread(aux, 1, lSize, f);
	
	if (lSize < 0) {
		printf("Error. There was an error while trying to read file\n");
		return NULL;
	}
	aux[lSize] = '\0';

	while (aux[i] != '\0') {
		if (aux[i] == '\n') {
			c++;
		}
		i++;
	}

	fclose(f);

	*lin = c;

	return aux;
}
