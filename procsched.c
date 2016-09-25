#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "lib_fichero.h"
#include "lib_cola.h"
#include <time.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0

void iniciar_pid_array(int * arr,int lng);
void actualizar_cola(proc_cola pc,int * a,int n);
void hijo_terminado(int signal, siginfo_t * info, void * data); //funcion a realizar cuando se capture la señal.

int main(int argc,char ** argv)
{
	process * aux;
	proc_cola proc;
	char * nom;
	int i,j;
	int * pid_array=NULL;
	int status;
	char buf[2];
	int fd;
	struct sigaction act;
	
	/*
		Preparamos la estructura que manejara sigaction cuando reciba la señal.
		Cuando reciba la señal, la funcion sigaction ejecutara la funcion definida
		en el campo sa_sigaction. La funcion hay que definirla mas adelante como si fuera
		una funcion normal. En los flags especificamos que use sig_action y no sig_hander y
		que además, omitiremos la señal SIGCHLD cuando el hijo la emita al parar el proceso.
		De esta forma nos aseguraremos de que solo ejecutara la accion cuando la señal SIGCHLD
		sea recibida al terminar el proceso hijo.
	*/
	
	act.sa_sigaction = hijo_terminado;
	act.sa_flags=SA_NOCLDSTOP | SA_SIGINFO;

	printf("\n");
	printf("\t\t\t\t******************************************************\n");
	printf("\t\t\t\t*		PLANIFICADOR DE TAREAS               *\n");
	printf("\t\t\t\t*		Version 0.1			     *\n");
	printf("\t\t\t\t******************************************************\n");
	printf("\n\n");
	
	if(argc < 2)
	{
		printf("Fichero no detectado. Introduce por teclado\n");
		printf("Ctrl + D para parar\n");
		nom = guarda_en_fichero();
		proc = carga_de_fichero(nom,LIPRST);
	}
	else
	{
		printf("Fichero detectado. Nombre: %s\n",argv[1]);
		proc = carga_de_fichero(argv[1],LIPRST);
	}
	if(!proc)
	{
		printf("Error. Las tareas no se han cargado correctamente\n");
		exit(0);
	}

	
	pid_array = malloc(sizeof(int)*num_tareas(proc));

	iniciar_pid_array(pid_array,num_tareas(proc));
	
	for(i=0;i<num_tareas(proc);i++) //lanzamos uno a uno todos los hijos y mientras es el padre el q los controla
	{
		aux = sacar(proc); //sacamos la tarea.
		avanzar(proc); //avanzamos la cola.
		encolar(proc,aux); //encolamos la tarea
		/*De esta forma podremos iniciar la tarea y pararla. Una tarea para cada hijo.*/
		pid_array[i] = fork();
		
		if(pid_array[i]==0) //proceso hijo.
		{
			signal(SIGSTOP,SIG_DFL); //programamos al hijo para que capture las señales de parada y continuar.
			signal(SIGCONT,SIG_DFL);
		  	raise(SIGSTOP); //hacemos dormir a todos los hijos hasta que todos estén cargados.
			sprintf(buf,"%d",i);
			fd=open(buf,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);//convierte la ruta(arg) en descriptor de fichero
			//con los flags definidos y el modo especificado.man open
			dup2(fd,STDOUT_FILENO);
			close(fd);
			execvp(aux->arg[0],aux->arg); //ejecutamos la tarea en cuestion
			exit(0); //esta funcion, termina el proceso y envia una señal al padre (SIGCHLD) para decirle q ha terminado
		}
	}
	i = num_tareas(proc);
	printf("\n%d Tareas cargadas :: %d Procesos lanzados \n",i,i);
	printf("Para continuar, pulse una tecla\n");
	getchar();
	
	actualizar_cola(proc,pid_array,num_tareas(proc));
	printf("\t\t\t\t EJECUCION \n");
	printf("*************************************************************************************\n");
	printf("* P = Pendientes *** PID = Process ID *** T = Tiempo *** E = Estado *** C = Comando *\n");
	printf("*************************************************************************************\n");
	printf("\n");
	j=0;
	while(!is_null(proc))
	{
		sigaction(SIGCHLD,&act,NULL); // preparamos al padre para capturar la señal y le decimos que accion debera realizar
					      // cuando capture la señal. En este caso, la señal SIGCHLD (se envia cuando el hijo para o termina.
		aux = sacar(proc); //sacamos de la pila.
		avanzar(proc); //avanzamos la pila.

		kill(aux->pid,SIGCONT); //enviamos la señal de continuar al hijo.
		printf("P: %d :: PID: %d :: T: %d sg :: E: Ejecutando :: C: %s %s \n",i,aux->pid,aux->tiempo,aux->arg[0],aux->arg[1]);
		
		sleep(aux->tiempo); //dormimos al padre tantos segundos como los leidos por el fichero.

		kill(aux->pid,SIGSTOP); //enviamos la señal de parar al hijo.
		printf("P: %d :: PID: %d :: T: %d sg :: E: Parado :: C: %s %s \n\n",i,aux->pid,aux->tiempo,aux->arg[0],aux->arg[1]);
		
		waitpid(aux->pid,&status,WUNTRACED | WCONTINUED); // ahora hacemos que el padre espere a que terminen los hijos.
								  // o bien, si los hijos no han terminado, que continue.
		if(WIFEXITED(status)) //Si status cumple la condicion de que el proceso haya terminado correctamente entonces entra.
		{
			printf("Proceso %d termino correctamente\n\n",aux->pid);
			i--;
			pid_array[j]=0;
		}
		else
		{
			encolar(proc,aux);
 		}
		j++;
	}
	printf("\n0 tareas pendientes. Fin del programa\n\n");
	free(pid_array);

	return 0;
	
}

void iniciar_pid_array(int * arr,int lng)
{
	int i;

	for(i=0;i<lng;i++)
		arr[i]=0;
}

void actualizar_cola(proc_cola pc,int * a,int n)
{
	int i=0;

	for(i=0;i<n;i++)
	{
		pc[i]->pid = a[i];
	}
}

/* 
Cuando capturemos la señal SIGCHLD, el programa ejecutara la funcion
hijo_terminado() que en esencia lo que hace es decir al padre que continue
pues esta dormido. de esta forma, nos aseguramos que si un proceso tiene un
tiempo de 10 segundos, pero termina en el segundo 2, el padre no se quede dormido
los 8 segundos restantes.
*/
void hijo_terminado(int signal, siginfo_t * info, void * data)
{
	raise(SIGCONT); //enviamos la señal al padre.
}

