#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fragmenta.h"

#define TRUE 1
#define FALSE 0

int buscar_inicio_cad(const char * str);

char ** fragmenta(const char * cadena)
{
	char * aux;
	char ** arg=NULL;
	int i=0,j=0,n,k=0,z,a=1,l=1;
	int pos=0;

	aux = cadena; // ya que cadena es de tipo const, necesitamos
		      // necesitamos operar con otra variable auxiliar
                      // para mover el puntero sin crear conflictos.

	pos = buscar_inicio_cad(cadena);

	aux+=pos; // avanzamos el puntero tantas veces como le indique
	i=j=0;    // el numero de posiciones.

	while(i<=strlen(aux))
	{
		arg = realloc(arg,sizeof(char *)*(a)); //modificamos el tamano del array de punteros a char.
		
		if(aux[i]==' ' || aux[i]=='\0')
		{
			
			if (j==0)
				l=i-j;
			else
				l=(i-1)-j;

			arg[k]=malloc(sizeof(char)*l); //reservamos la memoria donde guardaremos la cadena k.
			
			if(arg[k]==NULL)
				printf("Fallo critico: Memoria insuficiente\n\n");
			

			strncpy(arg[k],aux,sizeof(char)*l);
			
			z=0;
			for(n=j;n<i;n++)
			{
				arg[k][z]=aux[n];
				z=z+1;
			}
			arg[k][z]='\0';

			i++;
	
			while(aux[i]==' ') //subrutina que busca los espacios entre 2 palabras.
				i++;
			

			j=i;

			k++;
			a++;
		
		}
		else
			i++;
	}

	/*Tratamiento final:
	  Nos aseguramos de que en la ultima posicion tengamos un puntero
          a null, como se nos exige para el desarrollo de la practica.*/
	
	arg = realloc(arg,sizeof(char *)*(a+1));
	arg[k]=NULL;

	return arg;
	
}

/*Esta funcion busca el inicio de la primera letra de la cadena
  con esto conseguimos quitarnos los espacios que tenemos antes
  de la aparición de la primera letra.

  La función devuelte el indice donde se encuentra la primera letra
  o FALSE (0) en caso de que no la encuentre, en cuyo caso indicará
  que se le ha pasado un string vacio (pero con espacios)*/

int buscar_inicio_cad(const char * str) 
{
	int i=0;

	while(str[i] == ' ')
		i++;

	if(i==strlen(str))
		return FALSE;
	else
		return i;
}

/*A esta función se le pasa el doble puntero a char
  y va liberando la memoria. Por pasos, primero 
  libera la memoria apuntada por cada posicion de
  de la tabla y luego libera el puntero que apunta a la tabla.*/

void borrarg(char ** cadena)
{
	int i=0;
	while(!cadena[i])
		free(cadena[i]);
	free(cadena);
} 
