#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#define BUFFER_SIZE	5
#define NUM_PRODUCERS	2
#define NUM_CONSUMERS	1

char buffer[BUFFER_SIZE];
pthread_t producer[NUM_PRODUCERS];
pthread_t consumer[NUM_CONSUMERS];
int nextIn;
int nextOut;
sem_t notEmpty, notFull, producerMutex, consumerMutex;

void takeItem();
void placeItem();

void *producerRoutine(void *arg)
{
	while(1)
	{
		sleep(rand() %3 + 1);
		sem_wait(&notFull);
		sem_wait(&producerMutex);
		placeItem();
		sem_post(&producerMutex);
		sem_post(&notEmpty);
	}
}

void *consumerRoutine(void *arg)
{
	while(1)
	{
		sleep(rand() %3 + 1);
		sem_wait(&notEmpty);
		sem_wait(&consumerMutex);
		takeItem();
		sem_post(&consumerMutex);
		sem_post(&notFull);
	}
}

void takeItem()
{
	int i;
	sem_getvalue(&notEmpty, &i);
	printf("<<<ITEM TAKEN by %i, [%i] items in buffer\n", (((int)pthread_self())%100), i);
}

void placeItem()
{
	int i;
	sem_getvalue(&notEmpty, &i);
	printf(">>>ITEM PLACED by %i, [%i] items in buffer\n", (((int)pthread_self())%100), i+1);
}

int main(int argc, char *args[])
{
	int i;
	
	sem_init(&notEmpty, 0, 0);
	sem_init(&notFull, 0, BUFFER_SIZE);
	sem_init(&producerMutex, 0, 1);
	sem_init(&consumerMutex, 0, 1);
	sem_getvalue(&notFull,&i);
	printf("SEM VALUE: %i\n", i);

	for(i = 0; i < NUM_PRODUCERS; i++)
		pthread_create(&producer[i], NULL, producerRoutine, &i);
	for(i = 0; i < NUM_CONSUMERS; i++)
		pthread_create(&consumer[i], NULL, consumerRoutine, &i);
	while(1);
	return 0;
}
