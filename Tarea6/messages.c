#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>
#define CICLOS		10
#define MSG_SIZE	100

char *pais[3]={"Peru","Bolvia","Colombia"};

typedef struct mymsg
{
	long mtype;
	char mtext[MSG_SIZE];
} mensaj;

key_t key;
int msgqid;
int msgflg;
mensaj mensaje;

void proceso(int i)
{
	int k;
	int l;
	for(k=0;k<CICLOS;k++)
	{
		msgrcv(msgqid, &mensaje, 2, 0, 0);
		printf("Entra %s",pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf("- %s Sale\n",pais[i]);
		msgsnd(msgqid, &mensaje, 2, 0);
		// Espera aleatoria fuera de la sección crítica
		sleep(rand()%3);
	}
	exit(0);	// Termina el proceso
}

int main()
{
	int pid;
	int status;
	int shmid;
	int args[3];
	int i;
	void *thread_result;
	
	key = 1234;
	msgflg = IPC_CREAT|0666;

	if((msgqid = msgget(key, msgflg)) < 0)
	{
		perror("msgget failed");
		exit(1);
	}
	
	mensaje.mtype = 1;
	strcpy(mensaje.mtext, "OK");

	if(msgsnd(msgqid, &mensaje, 2, IPC_NOWAIT) < 0)
	{
		perror("msgsnd failed");
		exit(1);
	}
	
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
	// Eliminar la memoria compartida
}

