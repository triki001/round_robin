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
#include "parser.h"
#include "queue.h"

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
	char buf[20];
	int fd;
	struct sigaction act;

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
			signal(SIGSTOP,SIG_DFL);
			signal(SIGCONT,SIG_DFL);
		  	raise(SIGSTOP);

			sprintf(buf,"process_%d",i);
			fd=open(buf,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
			
			dup2(fd,STDOUT_FILENO);
			close(fd);

			execvp(aux->arg[0],aux->arg);
			printf("launching process > %s ## %s %s\n", aux->arg[0], aux->arg[0], aux->arg[1]);
			exit(0);
		} else {
			aux->pid = pid;
			queue_move(proc);
		}
	}
	pending_tasks = queue_size(proc);
	printf("\n%d Tasks Loaded :: %d Process running\n", pending_tasks, pending_tasks);
	printf("> Press any key to continue <\n");
	getchar();

	printf("*************************************************************************************\n");
	printf("* P = Pending *** PID = Process ID *** T = Time *** S = Status *** C = Command      *\n");
	printf("*************************************************************************************\n");
	printf("\n");

	while (!is_queue_empty(proc)) {
		sigaction(SIGCHLD,&act,NULL);
		aux = (process*)queue_next(proc);
		queue_move(proc);

		kill(aux->pid,SIGCONT);
		printf("P: %d :: PID: %d :: T: %d sg :: S: Executing :: C: %s %s \n",pending_tasks, aux->pid, aux->time, aux->arg[0], aux->arg[1]);

		sleep(aux->time);

		kill(aux->pid,SIGSTOP);
		printf("P: %d :: PID: %d :: T: %d sg :: E: Stopped :: C: %s %s \n\n",pending_tasks, aux->pid, aux->time, aux->arg[0], aux->arg[1]);

		waitpid(aux->pid,&status,WUNTRACED | WCONTINUED);

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

void child_finish_cb(int signal, siginfo_t* info, void* data)
{
	raise(SIGCONT); //enviamos la señal al padre.
}

