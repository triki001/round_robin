#include <stdio.h>
#include <signal.h>

typedef void (*funcPtr)();

int salir=0;    /* Falso */

void controlador( int *lista_Registros )
{
   int i;

   signal( SIGINT, (funcPtr)controlador );

   printf( "Error capturado\n\n" );
   for( i=0; i<11; i++ )
      printf( "\n[%d] = %d\t", i, lista_Registros[i] );
   salir=1;     /* Verdadero */
}

int main()
{
   float x, y=2.5, h=0.01;

   signal( SIGINT, (funcPtr)controlador );

   for( x=1.0; x<=100.0 && !salir; x+=h )
      printf( "%f / %f = %f\n", y, x, y/x );    /* Lista demasiada Larga */

   return 0;
}
