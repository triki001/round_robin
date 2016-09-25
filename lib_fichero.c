#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fragmenta.h"

#define TRUE 1
#define FALSE 0

struct proceso{ //definimos la estructura de datos a utilizar.
	int tiempo;
	int pid;
	char ** arg;
};

typedef struct proceso process; //definimos el tipo (para ahorrarnos tiempo)
// typedef process ** arrayProcess;
typedef process ** proc_cola;

static void guardar_en_estruc(process * procStr,char ** str);
static char * extraer_de_fichero(char * nombre,int * lin);
void * carga_de_fichero(char * nombre,int code);
static process ** carga_de_fichero_m1(char * nombre);
static char *** carga_de_fichero_m2(char * nombre);

/*
Esta funcion cogera los datos del fichero de configuracion
y los guardara en funcion de la constante pasada en la llamada.
LIMSTR = 2 -> Guardara el resultado en una tabla de tablas de strings.
LIPRST = 1 -> Guardara el resultado en una tabla de estructuras proceso que simularan una cola.

Esta funcion tiene varias partes:
1. Leer del fichero y guardar su contenido en un string (funcion extraer_de_fichero).
2. Realiza un recorrido por el string buscando los retornos de carro
	2.1. Si encuentra un retorno de carro trocea el string (funcion fragmenta)
	     y lo guarda en un array de strings.
	2.2. Se pasa dicho array a la funcion guardar_en_estruc para guardar su
	     contenido en la estructura.
	2.3. Si no se encuentra, sigue buscando.

  DECLARACION:  

  int carga_de_fichero(char * nombre,int code)

  FUNCIONAMIENTO:

  Esta es la funcion que se exportara fuera de la libreria.
  El funcionamiento es el explicado anteriormente.

  VALORES DE RETORNO:

  La funcion devuelve el puntero al array de estructuras


*/

void * carga_de_fichero(char * nombre,int code)
{
	process ** proc;
	char *** str;

	switch(code)
	{
		case 1:
		{
			proc = carga_de_fichero_m1(nombre);
			return proc;
		}
		break;
		case 2:
		{
			str = carga_de_fichero_m2(nombre);
			return str;
		}
		break;
		default:
			return NULL;
	}
}

/*
Esta funcion para la constante 1
*/

process ** carga_de_fichero_m1(char * nombre)
{
	char * aux=NULL, *str=NULL;
	char ** strM=NULL;
	process ** proc=NULL;
	int k,i=0,j;
	int lineas,x;
	
	aux = extraer_de_fichero(nombre,&lineas);
	
	proc = realloc(proc,sizeof(process *)*(lineas+1));

	if(proc==NULL)
		return NULL;
	j=0;
	k=1;
	x=0;
	while(aux[i]!='\0')
	{		
		if(aux[i]=='\n')
		{
			proc[j] = malloc(sizeof(process));

			if(proc[j]==NULL)
				return NULL;

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

	return proc;
}

/*
Esta funcion para la constante 2
*/

char *** carga_de_fichero_m2(char * nombre)
{

	char * aux=NULL, *str=NULL;
	char ** strM=NULL;
	char *** res=NULL;
	int k,i=0,j;
	int lineas,x;
	
	aux = extraer_de_fichero(nombre,&lineas);
	
	res = realloc(res,sizeof(char **)*(lineas+1));

	if(res==NULL)
		return NULL;
	j=0;
	k=1;
	x=0;

	while(aux[i]!='\0')
	{		
		if(aux[i]=='\n')
		{
			res[j] = malloc(sizeof(char **));

			if(res[j]==NULL)
				return NULL;

			str = malloc(sizeof(char)*(i+1));
			strncpy(str,aux,i);
			str[i]='\0';
			
			strM = fragmenta(str);
			res[j] = strM;

			free(str);

			aux=aux+i+1;
			i=0;j++;
		}
		else
			i++;
	}
	
	res[lineas]=NULL;

	return res;
}

char * guarda_en_fichero(char * str)
{
	FILE * f;
	char buf[2000];
	char * nombre="procsched.txt";

	f = fopen(nombre,"w");

	if(!f)
	{
		printf("Error. No se ha podido crear el archivo.\n\n");
		exit(0);
	}

	printf("> ");
	while(fgets(buf,2000,stdin)!=NULL)
	{
		fprintf(f,buf);
		printf("> ");
	}

	fclose(f);

	printf("\n");

	return nombre;
}

int num_tareas(process ** proc)
{
	int i=0;
	
	while(proc[i]!=NULL)
		i++;

	return i;

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
	
	f = fopen(nombre,"r");

	if(f==NULL)
	{
		printf("Error. Archivo %s no encontrado o protegido contra lectura/escritura\n\n",nombre);
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
