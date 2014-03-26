#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semaphores.h"
#define CICLOS 10

char *pais[3]={"Peru","Bolvia","Colombia"};
int semid;

void proceso(int i)
{
	int k;
	int l;
	for(k=0;k<CICLOS;k++)
	{
		semwait(semid);
		printf("Entra %s",pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf("- %s Sale\n",pais[i]);
		semsignal(semid);
		// Espera aleatoria fuera de la sección crítica
		sleep(rand()%3);
	}
	exit(0);	// Termina el proceso
}

int main()
{
	int pid;
	int status;
	int args[3];
	int i;
	void *thread_result;
	//crear semaforo
	key_t key = 1234;
	semid = createsem(key);
	if(semid == -1) perror("Couldn't create semaphore :(");
	seminit(semid, 1);

	srand(getpid());
	for(i=0;i<3;i++)
	{
		// Crea un nuevo proceso hijo que ejecuta la función proceso()
		pid=fork();
		if(pid==0)
			proceso(i);
	}
	for(i=0;i<3;i++)
		pid = wait(&status);
}

