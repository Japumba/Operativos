#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
// Macro que incluye el código de la instrucción máquina xchg
#define atomic_xchg(A,B) __asm__ __volatile__( \
				"  lock xchg %1,%0 ;\n" \
				: "=ir" (A)	     \
				: "m" (B), "ir" (A)  \
				);

#define CICLOS 10
char *pais[3]={"Peru","Bolvia","Colombia"};

sem_t semaphore;

void *hilo1(void *arg)
{
	int *mynum=(int *) arg;
	int i=*mynum;

	int k;
	int l;

	for(k=0;k<CICLOS;k++)
	{
		sem_wait(semaphore);
		// Inicia sección Crítica
		printf("Entra %s",pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf("- %s Sale\n",pais[i]);
		// Termina sección Crítica

		sem_post(semaphore);

		// Espera aleatoria fuera de la sección crítica
		sleep(rand()%3);
	}
}

int main()
{
	pthread_t tid[3];
	int res;
	int args[3];
	int i;
	void *thread_result;
	srand(getpid());

	// Crea los hilos
	for(i=0;i<3;i++)
	{
		args[i]=i;
		res = pthread_create(&tid[i], NULL, hilo1, (void *) &args[i]);
	}
	// Espera que terminen los hilos
	for(i=0;i<3;i++)
		res = pthread_join(tid[i], &thread_result);
}

