#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

/* Mini ejemplo de un round robin*/
void myfuncion(void);

int main(void)
{
	int pid,pid2,i,j,status;
	typedef int (*func)();
	struct timespec req,res;

	pid = fork(); //lanzamos al proceso hijo 1.

	if(pid==0)
	{
		raise(SIGSTOP); //le enviamos la señal de parar.
		signal(SIGSTOP,SIG_DFL); //programamos al hijo para que capture las señales de parada y continuar.
		signal(SIGCONT,SIG_DFL);
		kill(getppid(),SIGCHLD);
		for(i=0;i<200;i++) //programamos la tarea a realizar.
			for(j=0;j<500;j++)
				printf("Fila %d Columna %d\n",i,j);
		
	}
	else
	{
		pid2 = fork(); //lanzamos al proceso hijo 2.
		
		if(pid2==0)
		{
			raise(SIGSTOP); //le enviamos la señal de parar.
			signal(SIGSTOP,SIG_DFL); //programamos al hijo para que capture las señales de parada y continuar.
			signal(SIGCONT,SIG_DFL);

			for(i=0;i<200;i++) //programamos la tarea a realizar.
				for(j=0;j<500;j++)
					printf(" %d + %d = %d \n",i,j,i+j);
			
		}	
		else //este else corresponde al proceso padre. Este se encargará de trabajar con los procesos.
		{
			signal(SIGCHLD,myfuncion);
			printf("Procesos hijos parados. Pulse tecla para continuar\n");
			getchar();
			i=1;
			while(1)
			{
				if(i%2==0) //los turnos pares trabajara el proceso 2,
				{
					kill(pid2,SIGCONT); //le enviamos la señal de continuar.
					sleep(2); //dormimos al proceso padre 2 segundos.
					kill(pid2,SIGSTOP); //le enviamos la señal de parar.
				}
				else //los turnos impares el proceso 1.
				{
					kill(pid,SIGCONT); //le enviamos la señal de continuar.
					sleep(2); //dormimos al proceso padre 2 segundos.
					kill(pid,SIGSTOP); //le enviamos la señal de parar.
				}
				
				waitpid(-1,&status,WNOHANG); // ahora hacemos que el padre espere a que terminen los hijos.
							     // o bien, si los hijos no han terminado, que continue.
				if(WIFEXITED(status)) //Si status cumple la condicion de que el proceso haya terminado correctamente entonces entra.
					printf("Proceso termino correctamente\n");
				i++;
			}
		}
	}
	return 0;
}
void myfuncion(void)
{
	printf("HOLAAA\n");
	exit(0);
}
