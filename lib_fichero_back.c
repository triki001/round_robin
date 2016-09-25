#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fragmenta.h"

#define TRUE 1
#define FALSE 0

struct proceso{ //definimos la estructura de datos a utilizar.
	int tiempo;
	char ** arg;
};

typedef struct proceso process; //definimos el tipo (para ahorrarnos tiempo)
typedef process ** arrayProcess;

void guardar_en_estruc(process * procStr,char ** str);
int carga_de_fichero(char * nombre,process ** proc);
char * extraer_de_fichero(char * nombre,int * lin);

/*
Esta funcion cogera los datos del fichero de configuracion
y los guardara en un array de estructuras del tipo anterior.
Esta funcion tiene varias partes:
1. Leer del fichero y guardar su contenido en un string (funcion extraer_de_fichero).
2. Realiza un recorrido por el string buscando los retornos de carro
	2.1. Si encuentra un retorno de carro trocea el string (funcion fragmenta)
	     y lo guarda en un array de strings.
	2.2. Se pasa dicho array a la funcion guardar_en_estruc para guardar su
	     contenido en la estructura.
	2.3. Si no se encuentra, sigue buscando.

  DECLARACION:  

  int carga_de_fichero(char * nombre, process ** proc)

  FUNCIONAMIENTO:

  Esta es la funcion que se exportara fuera de la libreria.
  El funcionamiento es el explicado anteriormente.

  VALORES DE RETORNO:

  Esta funcion devuelve TRUE en caso de exito o FALSE en caso
  de fallo (normalmente por problemas con la reserva de memoria.


*/
int carga_de_fichero(char * nombre, process ** proc)
{
	FILE * f;
	char * aux=NULL, *str=NULL;
	char ** strM=NULL;
	int lSize,result,k,i=0,j;
	int lineas,x,t;
	
	aux = extraer_de_fichero(nombre,&lineas);
	
	proc = realloc(proc,sizeof(process *)*(lineas+1));

	if(proc==NULL)
		return FALSE;
	j=0;
	k=1;
	x=0;
	while(aux[i]!='\0')
	{		
		if(aux[i]=='\n')
		{
			proc[j] = malloc(sizeof(process));

			if(proc[j]==NULL)
				return FALSE;

			str = malloc(sizeof(char)*(i+1));
			strncpy(str,aux,i);
			str[i]='\0';
			
			strM = fragmenta(str);
			proc[j]->arg = malloc(sizeof(char *));

			guardar_en_estruc(proc[j],strM);

			free(str);

			aux=aux+i+1;
			i=0;j++;
		}
		else
			i++;
	}
	
	proc[lineas]=NULL;
	return TRUE;
}

/* 
  DECLARACION:  

  void guardar_en_estruc(SAL process * procStr,ENT char ** str)

  FUNCIONAMIENTO:

  Esta funcion guarda el contenido del array de strings en una estructura de datos.
  dicha variable es de salida por lo que necesitaremos reservar previamente un espacio
  fuera de la funcion y luego redimensionarla si fuera necesario (dentro de la funcion)

  VALORES DE RETORNO:

 esta funcion no retorna nada. Solo el parametro de salida.
*/

void guardar_en_estruc(process * procStr,char ** str)
{
	int t,j,i=0;
	char * aux;

	sscanf(str[0],"%d",&t);
	procStr->tiempo=t;
	str++;
	while(str[i]!=NULL)
	{
		j=i+1;
		procStr->arg = realloc(procStr->arg,sizeof(char *)*j);
		procStr->arg[i] = malloc(sizeof(char)*strlen(str[i]));
		strcpy(procStr->arg[i],str[i]);
		i++;
	}
	j=i+1;
	procStr->arg = realloc(procStr->arg,sizeof(char *)*j);
	procStr->arg[i]=NULL;
}

/* 
  DECLARACION:  

  char * extraer_de_fichero( ENT char * nombre,E/S int * lin);

  FUNCIONAMIENTO:

  Esta funcion abre el fichero expecificado por la variable "nombre" y
  vuelva el contenido del fichero sobre un string para su posterior
  manipulacion. Ademas esta funcion calcula el numero de lineas del fichero
  que sera almacenada en la variable de E/S lin.

  VALORES DE RETORNO:

  esta funcion devuelve un string con el contenido del fichero.
*/

char * extraer_de_fichero(char * nombre,int * lin)
{
	FILE * f;
	int lSize,i=0,result,c=0;
	char * aux;
	char * out;

	f = fopen(nombre,"r");

	if(f==NULL)
	{
		printf("ERROR!!!!!!");
		exit(0);
	}

	fseek(f,0,SEEK_END); //buscamos el final del archivo.
	lSize=ftell(f); //obtenemos el tamaño del archivo.
	rewind(f); //rebobinamos el fichero
	
	aux = malloc(sizeof(char)*(lSize+1)); //reservamos una variable con el tamaño del fichero.
	result = fread (aux,1,lSize,f); // leemos del fichero desde el inicio hasta el final (almacenando los datos).
	aux[lSize]='\0';

	while(aux[i]!='\0') //con este bucle calculamos las lineas que tiene el fichero.
	{
		if(aux[i]=='\n')
			c++;
		i++;
	}
	fclose(f);

	*lin = c;
	
	return aux;
}
