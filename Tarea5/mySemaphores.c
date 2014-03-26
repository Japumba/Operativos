#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#define CICLOS		10
#define Q_SIZE		20
#define BASETIME	0
#define atomic_xchg(A,B)	__asm__ __volatile__(		\
					"lock xchg %1,%0 ;\n"	\
					: "=ir" (A)		\
					: "m" (B), "ir" (A)	\
					);

typedef struct struct_semaphore
{
	int count;
	int queue[Q_SIZE];
} mySemaphore;

void waitsem(mySemaphore *sem);
void signalsem(mySemaphore *sem);
void pauseprocess(int pid);
void resumeprocess(int pid);
int dequeue(mySemaphore *sem);
void enqueue(mySemaphore *sem, int pid);
int initsem(mySemaphore *sem, int value);
void printqueue(mySemaphore *sem);

char *pais[3]={"Peru    ","Bolivia ","Colombia"};

int *g;
mySemaphore *mySem;

void proceso(int i)
{
	int k;
	for(k=0;k<CICLOS;k++)
	{
		waitsem(mySem);
		printf("Entra %s %i°",pais[i], k+1);
		fflush(stdout);
		sleep(rand()%3 + BASETIME);
		printf(" - %s Sale\n",pais[i]);

		signalsem(mySem);

		// Espera aleatoria fuera de la sección crítica
		sleep(rand()%3 + BASETIME);
	}
	exit(0);	// Termina el proceso
}

int main()
{
	int pid;
	int status;
	int shmidG, shmidSEM;
	int args[3];
	int i;
	void *thread_result;
	// Declarar memoria compartida
	shmidG=shmget(0x1234,sizeof(g),0666|IPC_CREAT);
	shmidSEM=shmget(0x1235, sizeof(mySem),0666|IPC_CREAT);
	if(shmidG==-1 || shmidSEM==-1)
	{
		perror("Error en la memoria compartida\n");
		exit(1);
	}
	
	g=shmat(shmidG,NULL,0);
	mySem=shmat(shmidSEM,NULL,0);

	if(g==NULL || mySem==NULL)
	{
		perror("Error en el shmat\n");
		exit(2);
	}

	*g=0;
	initsem(mySem, 1);
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
	shmdt(g);
	shmdt(mySem);
}

//nuestras cosas de semaforos

void waitsem(mySemaphore *sem)
{
	register int l =1;
	do{atomic_xchg(l,*g);}while(l != 0);
	if(sem->count <=0)
	{
		//printf("Ocupado, el proceso %i se va a la cola\n", getpid());
		enqueue(sem, getpid());
		sem->count --;
		*g = 0;
		pauseprocess(getpid());
	}else
	{
		//printf("No ocupado! el proceso %i se ejecuta\n", getpid());
		sem->count --;
		*g =0;
	}
}

void enqueue(mySemaphore *sem, int pid)
{
	sem->queue[-(sem->count)] = pid;
	//printqueue(sem);
}

void signalsem(mySemaphore *sem)
{
	register int l =1;
	do{atomic_xchg(l,*g);}while(l != 0);

	if(sem->count <0)
	{
		//printf("El proceso %i termina! Le toca al siguiente\n", getpid());
		resumeprocess(dequeue(sem));
	}
	sem->count ++;
	*g = 0;
}

void pauseprocess(int pid)
{
	kill(pid, SIGSTOP);
}

void resumeprocess(int pid)
{
	kill(pid, SIGCONT);
}

int dequeue(mySemaphore *sem)
{
	int val;
	if(sem->count >= 0)
		return -1;

	val = sem->queue[0];
	int i;
	for(i = 0; i < Q_SIZE-1; i++)
		sem->queue[i] = sem->queue[i+1];
	sem->queue[Q_SIZE-1] = -1;
	//printf("Le toca al proceso %i ejecutarse\n", val);
	//printqueue(sem);
	return val;
}

int initsem(mySemaphore *sem, int val)
{
	if(val < 0)
		return 0;
	
	sem->count = val;
	int i;
	for(i = 0; i < Q_SIZE; i++)
		sem->queue[i] = -1;
	return 1;
}

void printqueue(mySemaphore *sem)
{
	int *cur;
	cur = sem->queue;
	printf("queue: ");
	while(*cur != -1)
	{
		printf("[%i]",*cur);
		cur++;
	}
	printf("\n");
}

