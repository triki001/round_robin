
#ifndef _LIB_FICHERO_H_
#define _LIB_FICHERO_H_

#define LIMSTR 2
#define LIPRST 1

/*
	Estas constantes significan: 
		LIMSTR Load In MatrixString -> Carga el fichero en un puntero a tablas de caracteres.
		LIPRST Load In ProcessSTruct -> Carga el fichero en una "cola" de estructuras de tipo proceso.

	Hemos decidido hacer esto por si necesitamos cargar un fichero en un tipo cadena de caracteres
	o bien queremos que lo cargue en una cola.

*/

struct process_ {
	int time;
	int pid;
	char** arg;
};

typedef struct process_ process;
typedef process** proc_queue;


process** load_from_file(const char* filename);
int num_tasks(const proc_queue proc);
char* save_to_file();
void free_processes(proc_queue proc);
#endif /*_LIB_FICHERO_H_*/
