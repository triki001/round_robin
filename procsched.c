#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include "lib_fichero.h"
#include "lib_cola.h"

#define TRUE 1
#define FALSE 0

void child_finish_cb(int signal, siginfo_t* info, void* data);

int main(int argc,char** argv)
{
	process* aux = NULL;
	proc_queue* proc = NULL;
	int pending_tasks;
	int pid;
	int i;
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
	
	act.sa_sigaction = child_finish_cb;
	act.sa_flags=SA_NOCLDSTOP | SA_SIGINFO;

	printf("\n");
	printf("\t\t\t\t******************************************************\n");
	printf("\t\t\t\t*           ROUND ROBIN PROCESS SCHEDULER            *\n");
	printf("\t\t\t\t*		Version 0.1			     *\n");
	printf("\t\t\t\t******************************************************\n");
	printf("\n\n");
	
	if (argc != 2) {
		printf("Error. Unable to find the process file. Abort.\n");
		exit(-1);
	} else {
		printf("> Loading file: %s\n",argv[1]);
		proc = load_from_file(argv[1]);
	}

	if (!proc) {
		printf("Error. Unable to load the tasks defined in file\n");
		exit(-2);
	}
	
	for (i = 0; i < queue_size(proc); i++) {
		aux = queue_next(proc); //sacamos la tarea.
		/*De esta forma podremos iniciar la tarea y pararla. Una tarea para cada hijo.*/
		pid = fork();
		
		if (pid == 0) {
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
		} else {
			aux->pid = pid;
			queue_move(proc); //avanzamos la cola.
		}
	}
	pending_tasks = queue_size(proc);
	printf("\n%d Tareas cargadas :: %d Procesos lanzados \n", pending_tasks, pending_tasks);
	printf("Para continuar, pulse una tecla\n");
	getchar();
	
	//update_queue(proc, pid_array, num_tasks(proc));
	printf("*************************************************************************************\n");
	printf("* P = Pending *** PID = Process ID *** T = Time *** S = Status *** C = Command      *\n");
	printf("*************************************************************************************\n");
	printf("\n");

	while (!is_queue_empty(proc)) {
		sigaction(SIGCHLD,&act,NULL); // preparamos al padre para capturar la señal y le decimos que accion debera realizar
					      // cuando capture la señal. En este caso, la señal SIGCHLD (se envia cuando el hijo para o termina.
		aux = (process*)queue_next(proc); //sacamos de la pila.
		queue_move(proc); //avanzamos la pila.

		kill(aux->pid,SIGCONT); //enviamos la señal de continuar al hijo.
		printf("P: %d :: PID: %d :: T: %d sg :: S: Executing :: C: %s %s \n",pending_tasks, aux->pid, aux->time, aux->arg[0], aux->arg[1]);
		
		sleep(aux->time); //dormimos al padre tantos segundos como los leidos por el fichero.

		kill(aux->pid,SIGSTOP); //enviamos la señal de parar al hijo.
		printf("P: %d :: PID: %d :: T: %d sg :: E: Stopped :: C: %s %s \n\n",pending_tasks, aux->pid, aux->time, aux->arg[0], aux->arg[1]);
		
		waitpid(aux->pid,&status,WUNTRACED | WCONTINUED); // ahora hacemos que el padre espere a que terminen los hijos.
								  // o bien, si los hijos no han terminado, que continue.
		if (WIFEXITED(status)) {
			printf("Process %d finished successfully.\n\n",aux->pid);
			queue_remove(proc);
			pending_tasks = queue_size(proc);
		} else {
			queue_move(proc);
 		}
	}
	printf("\nNo more pending processes. Finish.\n\n");
	queue_free(&proc, free_process_data);

	return 0;
	
}

/* 
Cuando capturemos la señal SIGCHLD, el programa ejecutara la funcion
hijo_terminado() que en esencia lo que hace es decir al padre que continue
pues esta dormido. de esta forma, nos aseguramos que si un proceso tiene un
tiempo de 10 segundos, pero termina en el segundo 2, el padre no se quede dormido
los 8 segundos restantes.
*/
void child_finish_cb(int signal, siginfo_t* info, void* data)
{
	raise(SIGCONT); //enviamos la señal al padre.
}

