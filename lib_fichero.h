
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

struct proceso{ //definimos la estructura de datos a utilizar.
	int tiempo;
	int pid;
	char ** arg;
};

typedef struct proceso process;
typedef process ** proc_cola;


void * carga_de_fichero(char * nombre,int code);
int num_tareas(process ** proc);
char * guarda_en_fichero();

#endif /*_LIB_FICHERO_H_*/
